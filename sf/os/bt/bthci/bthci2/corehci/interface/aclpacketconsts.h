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

#ifndef ACLPACKETCONSTS_H
#define ACLPACKETCONSTS_H

#include <e32def.h>

enum TAclPacketBoundaryFlag
	{
    EContinuingHLFragment = 0x01,
    EFirstHLFragment = 0x02,
	};

enum TAclPacketBroadcastFlag
	{
	EPointToPointOnly = 0x00,
	EActiveSlaveBroadcast = 0x01,
	EParkedSlaveBroadcast = 0x02,
	};

// ACL Data Packet Flags
const TUint8 KPacketDataFlagMask = 0xf0; /*!<Packet Data Flag Mask.  Operates on high connection handle byte. */
const TUint8 KPacketPBFlagMask = 0x03; /*!< Packet Boundary Flag Mask.  Operates on bottom two bits of byte - 00xx. */
const TUint8 KPacketBCFlagMask = 0x0C; /*!< Packet Broadcast Flag Mask. Operates on top two bits of bottom nibble - xx00. */
const TUint8 KPacketPBtoBCFlagShift = 2; /*!< The shift needed for the broadcast flag relative to the boundary flag. */
const TUint8 KPacketPBBCFlagShift = 4; /*!< Packet Boundary and Broadcast Flag Shift.  */


// Constants for source compatability.

// Packet Boundary Flags
const TUint8 KContinuingHLFragment = EContinuingHLFragment; /*!< Continuing higher layer fragment. */
const TUint8 KFirstHLFragment = EFirstHLFragment; /*!< First higher layer fragment. */

// Packet Broadcast Flags
const TUint8 KPointToPointOnly = EPointToPointOnly; /*!< Point to point only broadcast flag. */
const TUint8 KActiveBroadcast = EActiveSlaveBroadcast; /*!< Active broadcast flag. */
const TUint8 KPiconetBroadcast = EParkedSlaveBroadcast; /*!< Piconet broadcast flag. */

#endif // ACLPACKETCONSTS_H
