// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This header file holds 2G/3G specific bearer type information.
// These values must be used by the NetMcpr/Licensee NetMcpr to 
// calculate the TCP window for a connection. 
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __ETEL_BEARERS_H_
#define __ETEL_BEARERS_H_


/**
 * Identifies GPRS
 */
const TUint KGprsBearer				= 20;  

/**
 * Identifies EDGE
 */
const TUint KEGprsBearer			= 21;  

/**
 * Identifies UMTS
 */
const TUint KUmtsBearer				= 22;  

/**
 * Identifies HSDPA
 */
const TUint KHsdpaBearer			= 23;   

#endif //__ETEL_BEARERS_H_
