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

/**
* @file
* @publishedPartner
* @prototype
*/


#ifndef LBS_LOCATION_CLASS_TYPES_H
#define LBS_LOCATION_CLASS_TYPES_H


/** 
 * Type used for identifying location info class types 
 */
typedef TUint32 TLbsAreaInfoClassType;

/**
 * Enumeration to indicate the type of area information.
 * This information is included when a client application creates
 * a class derived from CLbsAreaInfoBase
 */
enum _TLbsAreaInfoClassType
    {
    /** 
     * Should be used for data initialisation only 
     */
    ELbsAreaInfoUnknownClass                = 0x00,
    
    /** 
     * Identifier for CLbsPositionInfo class 
     */
    ELbsAreaPositionInfoClass             = 0x01,
    
    /** 
     * Identifier for CLbsGsmCellInfo class 
     */
    ELbsAreaGsmCellInfoClass                = 0x02,
    
    /** 
     * Identifier for CLbsGsmNeighbourCellInfo class 
     */
    ELbsAreaGsmNeighbourCellInfoClass       = 0x04,
    
    /** 
     * Identifier for CLbsWcdmaCellInfo class 
     */
    ELbsAreaWcmdaCellInfoClass              = 0x08,
    
    /**
     *  Identifier for CLbsWcdmaNeighbourCellInfo class 
     */
    ELbsAreaWcmdaNeighbourCellInfoClass     = 0x10,
    
    /** 
     * Identifier for CLbsWlanInfo class 
     */
    ELbsAreaWlanInfoClass                   = 0x20,
    
    /** 
     * Not used 
     */
    ELbsAreaInfoLastClass                   = 0xFFFFFFFF
    };


/** 
 * Type used for identifying location info class types 
 */
typedef TUint32 TLbsLocationInfoClassType;

/**
 * Enumeration to indicate the type of area information.
 * This information is included when a client application creates
 * a class derived from CLbsLocationInfoBase
 */
enum _TLbsLocationInfoClassType
    {
	/** 
	 * Should be used for data initialisation only 
	 */
	ELbsLocationInfoUnknownClass    = 0x00,

	/** 
	 * Identifier for CLbsLocationInfo class
	 */
    ELbsLocationInfoClass           = 0x01,

	/** 
	 * Not used
	 */
	ELbsLocationInfoLastClass       = 0xFFFFFFFF
    };

#endif //LBS_LOCATION_CLASS_TYPES_H
