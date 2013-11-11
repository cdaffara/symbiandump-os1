// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFCOMMON_H
#define MDFCOMMON_H

/**
Control flags for tunneling
*/
enum TTunnelFlags
	{
	/** 
	The input port receiving a buffer cannot alter its contents when tunnelled.
	*/
	EBufferReadOnly = 1
	};

/**
Defines the supplier of the buffers used in a tunnel
*/
enum TSupplierType
	{
	/** 
 	Port supplying the buffers is unspecified, or don't care 
 	*/
    EBufferSupplyUnspecified = 0,
	/**
 	Input port supplies the buffers 
 	*/
	EBufferSupplyInput,             
	/**
 	Output port supplies the buffers
 	*/
	EBufferSupplyOutput
	};

/**
Errors used by the Codec API
*/
enum TCodecApiErrors
	{
	/** 
 	Invalid state
 	*/
	EInvalidState = 0,
	/** 
 	Port already tunnelled
 	*/
	EPortAlreadyTunnelled
	};

#endif // MDFCOMMON_H
