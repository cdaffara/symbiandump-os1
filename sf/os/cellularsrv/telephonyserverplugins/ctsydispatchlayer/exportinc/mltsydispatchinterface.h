// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef MLTSYDISPATCHINTERFACE_H_
#define MLTSYDISPATCHINTERFACE_H_

#include <e32std.h>
#include <e32def.h>

typedef const TInt TLtsyCallbackIndId;
typedef const TInt TLtsyDispatchInterfaceApiId;

// Offsets of IDs used to identify API calls and also indicator callbacks
const TInt KDispatchCallControlFuncUnitId = 0;
const TInt KDispatchPhoneFuncUnitId = 2000;
const TInt KDispatchSecurityFuncUnitId = 4000;
const TInt KDispatchPhonebookEnFuncUnitId = 6000;
const TInt KDispatchCellBroadcastFuncUnitId = 8000;
const TInt KDispatchPhonebookOnFuncUnitId = 10000;
const TInt KDispatchPhonebookFuncUnitId = 12000;
const TInt KDispatchSimFuncUnitId = 14000;
const TInt KDispatchSmsFuncUnitId = 16000;
const TInt KDispatchCallControlMultipartyFuncUnitId = 18000;
const TInt KDispatchSupplementaryServicesFuncUnitId = 20000;
const TInt KDispatchPacketServicesFuncUnitId = 22000;
const TInt KDispatchSatFuncUnitId = 24000;

/**
 * Identifies the group that a set of IDs belong to.  
 * Used with MLtsyDispatchFactoryV1::IsCallbackIndicatorSupported
 *
 * @see MLtsyDispatchFactoryV1::IsCallbackIndicatorSupported
 */
enum TLtsyDispatchIndIdGroup
	{	
	EIndIdGroup1, //< Callback indicator IDs 1 to 32
	EIndIdGroup2, //< Callback indicator IDs 33 to 63
	EIndIdGroup3  //< Callback indicator IDs 64 to 95
	};

class MLtsyDispatchInterface
/**
 * Base class from which all dispatch interfaces to be implemented by the Licensee LTSY
 * derive.
 */
	{
public:


	}; // class MLtsyDispatchInterface

#endif /*MLTSYDISPATCHINTERFACE_H_*/
