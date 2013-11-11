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


#ifndef LBS_LOC_INFO_CONV_PLUGIN_CLASS_TYPES_H
#define LBS_LOC_INFO_CONV_PLUGIN_CLASS_TYPES_H


/** Type used for identifying class types */
typedef TUint32 TLbsLocInfoConverterPluginParamsClassType;


/**
 * Class types for TLbsLocInfoConverterPluginParams and its derived classes 
 */
enum _TLbsLocInfoConverterPluginParamsClassType
	{
	/** Should be used for data initialisation only */
	ELbsLocInfoConverterPluginParamsUnknownClass = 0x00,

	/** Identifier for TLbsLocInfoConverterPluginParams class */
	ELbsLocInfoConverterPluginParamsClass        = 0x01,

	/** Not used. */
	ELbsLocInfoConverterPluginParamsLastClass    = 0xFFFFFFFF
	};


#endif // LBS_LOC_INFO_CONV_PLUGIN_CLASS_TYPES_H

