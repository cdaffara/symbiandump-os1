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
* Description: Common enumerations used by client applications and plugins 
* for conversion of location information.
*
*/

/**
@file
@publishedPartner
@prototype
*/

#ifndef LBS_LOCATION_INFO_CONVERTER_COMMON_H
#define LBS_LOCATION_INFO_CONVERTER_COMMON_H

#include <e32std.h>

/**
 * Conversion preferences
 */ 
typedef TUint32 TLbsConversionPrefs;


/**
 * Enumeration to define the preferences a client can specify while requesting
 * location information. The conversion preferences enumeration shall not be 
 * used as a bitmask. 
 * The preferences are used by conversion module in order to decide if/how
 * a connection to an external server should be established.
 */
enum _TLbsConversionPrefs
    {
    	
    /**
     * Indicates that no preferences are specified.
     */
    ELbsConversionNotDefined = 0x0000,
    	
    /**
     * Use only local cache to retrieve information.
     */
    ELbsConversionCacheOnly  = 0x0001,
  
    /**
     * Disable prompt while making a connection to an external service for
     * conversion.
	 * Ignored if a conversion module does not require external connection.
     */
    ELbsConversionSilent     = 0x0002,
  
    /**
     * Allow prompt while making a connection to an external service for 
     * conversion.
     * * Ignored if a conversion module does not require external connection.
     */
    ELbsConversionPrompt     = 0x0004,
  
    /**
     * 
     * Allow Wlan Only to make a connection only using W-Lan.
     * 
     */
    ELbsWlanOnly             = 0x0008

    };  


/**
 * Output Location Information
 */ 
typedef TUint32 TLbsConversionOutputInfoMask;


/**
 * Enumeration to define the type(s) of location information being communicated
 */
enum _TLbsConversionOutputInfoMask
    { 
    
    /**
     * Used to initialise data only. Should not be used when requesting conversions.
     */
    ELbsConversionOutputNotDefined = 0x0000,

    /**
     * Indicates that the input data should be converted to a geographical coordinate
	 * @see CLbsPositionInfo
     */
    ELbsConversionOutputPosition = 0x0001,
    
    /**
     * Indicates that the input data should be converted to a GSM Cell information 
	 * @see CLbsGsmCellInfo
     */
    ELbsConversionOutputGsm = 0x0002,
    
    /**
     * Indicates that the input data should be converted to a WCDMA Cell information 
	 * @see CLbsWcdmaCellInfo
     */
    ELbsConversionOutputWcdma = 0x0004,
    
    /**
     * Indicates that the input data should be converted to a WLAN information 
	 * @see CLbsWlanInfo
     */
    ELbsConversionOutputWlan = 0x0008
    };

#endif // LBS_LOCATION_INFO_CONVERTER_COMMON_H
