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
* Description:  Definition of Movement Detection API
*
*/


#ifndef C_TESTLOCATIONCONVERTER_H
#define C_TESTLOCATIONCONVERTER_H

#include <e32base.h>

#include <commdb.h>
#include <lbs/lbslocinfoconverterpluginbase.h>
#include <lbs/lbslocinfoconverterpluginclasstypes.h>
#include <lbs/lbslocinfoconverterplugincommon.h>

#include <lbs/lbslocationinfoconvertercommon.h>
#include <lbs/lbslocationclasstypes.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationposinfo.h>
#include <lbs/lbslocationgsminfo.h>
#include <lbs/lbslocationgsmneighbourinfo.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationwcdmainfo.h>
#include <lbs/lbslocationwcdmaneighbourinfo.h>
#include <lbs/lbslocationwlaninfo.h>

/**
 *  CTestLocationConverter is an ECom plugin that implements CLbsLocInfoConversionPluginBase.
 *
 *  This class is responsible for monitering movement of mobile based on
 *  GSM information available to mobile at any instant.
 *
 *  @lib movementdetector.lib
 *  @since  S60 v4.0
 */
class CTestLocationConverter : public CLbsLocInfoConverterPluginBase
										
    {

public:

    /**
     * Loads a location info conveter and initalises it.
     * 
     * @param[in] aObserver An observer for location conversion event.
     * @param[in] aPluginArgs Agruements sent to conversion plugin.
     * return Pointer to the new loation info converter.If module id is not specified
     * default plug in will be loaded.
     */
    static CTestLocationConverter* NewL( TLbsLocInfoConverterPluginParams& aPluginParams );

    /**
     * Unloads this movement detector.
     *
     * @since S60 TACO
     */
     virtual ~CTestLocationConverter();

	// From CLbsLocInfoConversionPluginBase

     /**
          * Converts the area informations of a group of geographical locations
          * to its corresponding position information.
          * 
          * @param[in] aLocationInfoArray Array of location informations corresponding 
          * to geographical locations are passed and on successful completion of this 
          * method,position information corresponding to the loation informations are  
          * updated in each area info object.
          * @param[in] aConversionPrefs Conversion preferences suggested by client.
          * 
          * @leave KErrArgument If the location info array is empty.
          */
     virtual void ConvertLocationInfoL( 
             RLbsLocationInfoArray& aLocationInfoArray,
             const TLbsConversionPrefs aConversionPrefs, 
             const TLbsConversionOutputInfoMask aRequestedInfo );

     /**
      * Cancels get location info operation.
      */     
     virtual void CancelConvertLocationInfo();
     
     virtual void ValidateClient( const TSecureId& aSecureId, 
                                  const TVendorId& aVendorId,
                                  const TLbsConversionOutputInfoMask aRequestedInfo );
                                  
     virtual void CancelValidateClient();        
     
     virtual void ConversionPluginPreferences(
                 TLbsConversionPluginInfo& aPluginInfo );

private:

	
	 void ConstructL(TLbsLocInfoConverterPluginParams& aPluginParams);
	
	/**
	 * Default C++ Constructor
	 */
	 CTestLocationConverter(TLbsLocInfoConverterPluginParams& aParams);
	 
private: // data

    MLbsLocInfoConverterPluginObserver &iObserver;
    TBool iRetainLocalityInCache;
    };


#endif // C_TESTLOCATIONCONVERTER_H
