// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HCTLPOWERINTERFACE_H
#define HCTLPOWERINTERFACE_H

#include <bluetooth/hci/hcitypes.h>		// for TBTPowerState

/**
The UID of the power interface.
*/
const TInt KHCTLPowerInterfaceUid = 0x102797ab;

/**
Mixin for the power interface.

This interface is to be accessable via the core HCTL plugin API.
@see CHCTLBase::Interface(TUid)

It provides an example route by which the Core HCI could tell the HCTL to
make controller power state requests.
*/ 
class MHCTLPowerInterface
	{
public:
	/**
	Request snap shot of current power state
	
	@param aPowerState On/Off @see TBTPowerState
	@return Error Code
	*/
	virtual TInt MhpiGetPower(TBTPowerState& aPowerState) = 0;

	/**
	Request change to power state
	
	@param aPowerState On/Off @see TBTPowerState
	@return Error Code
	*/
	virtual TInt MhpiSetPower(TBTPowerState aPowerState) = 0;
	};

#endif //HCTLPOWERINTERFACE_H
