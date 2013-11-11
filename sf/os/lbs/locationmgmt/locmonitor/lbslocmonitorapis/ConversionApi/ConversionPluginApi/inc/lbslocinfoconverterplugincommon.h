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
* Description: Base class for all the class representing
* location informations.
*
*/

/**
@file
@publishedPartner
@prototype
*/

#ifndef LBS_LOCATION_CONVERTER_PLUGIN_COMMON_H
#define LBS_LOCATION_CONVERTER_PLUGIN_COMMON_H

#include <e32std.h>


/**
The generic base for classes that store conversion Plugin Interface 
related information
*/
class TLbsLocInfoConverterPluginClassTypeBase
	{
public:
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	IMPORT_C TLbsLocInfoConverterPluginClassTypeBase();

protected:
	/** The type of the derived class */
	TUint32 iClassType;
	/** The size of the derived class */
	TUint iClassSize;
	};

/**
 * Conversion plugin's capabilities
 */
typedef TUint32 TLbsConversionPluginCapabilityMask;

/**
 * Enums that are used as bitmasks to define the capabilities of the conversion plugin.
 */
enum _TLbsConversionPluginCapabilityMask
    {
    /**
     * Bitmask indicating the conversion plugin's capability has not been defined.
     */
    ELbsPluginCapabilityNotDefined = 0x00,
            
    /**
     * Bitmask indicating the conversion plugin's capability to use position information
     * for the conversion.
     */
    ELbsPluginCapabilityPosInfo = 0x01,
    
    /**
     * Bitmask indicating the conversion plugin's capability to use GSM cell information
     * for the conversion. 
     */
    ELbsPluginCapabilityGsmInfo = 0x02,
    
    /**
     * Bitmask indicating the conversion plugin's capability to use WCDMA cell information
     * for the conversion. 
     */
    ELbsPluginCapabilityWcdmaInfo = 0x04,
    
    /**
     * Bitmask indicating the conversion plugin's capability to use WLAN AP information
     * for the conversion.
     */
    ELbsPluginCapabilityWlanInfo = 0x08,
    
    /**
     * Bitmask indicating the conversion plugin's capability to use all area information
     * for the conversion.
     */
    ELbsPluginCapabilityAreaInfoAll = 0xFFFFFFFF
    };


/**
 * Class used to read the preferences of the conversion plugin such as its capability to use
 * gsm/wlan/wcdma information during conversion and whether the position that it returns is to be 
 * stored in the cache.
 */
class TLbsConversionPluginInfo
    {
public:
    IMPORT_C TLbsConversionPluginInfo();

public:
    /**
     * Read the flag indicating if the conversion result is to be cached
     */
    IMPORT_C TBool IsPluginPositionToBeCached() const;
    
    /**
     * Read the currently set capabilities
     */
    IMPORT_C TLbsConversionPluginCapabilityMask PluginCapabilitiesMask() const;
    
    /**
     * Sets the flag indicating if the conversion result returned from the plugin is 
     * to be cached.
     */
    IMPORT_C void  SetIsPluginPositionToBeCached(TBool aCachePluginPosition);
    
    /**
     * Sets the bitmask representing the capabilities of the conversion plugin.
     */
    IMPORT_C void  SetPluginCapabilitiesMask(TLbsConversionPluginCapabilityMask aCapabilitiesMask);

private:
    TBool                               iIsPluginPositionToBeCached;
    TLbsConversionPluginCapabilityMask  iCapabilitiesMask;
    TUint32                             iReserved[8];
    };

#endif // LBS_LOCATION_CONVERTER_PLUGIN_COMMON_H
