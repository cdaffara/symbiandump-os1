// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
@internalTechnology
@released
*/

#ifndef BTSCOPARAMS_H
#define BTSCOPARAMS_H

#include <btsyncbandwidth.h>

/**
@internalTechnology
Used internally to hold eSCO link parameters.  Not intended for use.
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
NONSHARABLE_CLASS(TBTeSCOLinkParams)
	{
	public:
		TBTeSCOLinkParams() {};
		TBTeSCOLinkParams(TUint aBandwidth, TUint16 aCoding, TUint16 aLatency, TUint8 aRetransmission);
		
		TBTSyncBandwidth iBandwidth;
		TUint16 iCoding;
		TUint16 iLatency;
		TUint8 iRetransmissionEffort;	
	};

enum TSCOType
/** Bluetooth link SCO type
@internalTechnology
WARNING: For internal use ONLY. Compatibility is not guaranteed in future releases.
*/
	{
	ESCO=0x01,	/*!< Synchronous Connection Oriented link */
	EeSCO=0x02	/*!< eSCO link */
	};	

#endif // BTSCOPARAMS_H
