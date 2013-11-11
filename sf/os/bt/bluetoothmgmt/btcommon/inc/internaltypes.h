// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef INTERNALTYPES_H
#define INTERNALTYPES_H

#include <bt_sock.h>

const TUint KSolBtSCO 	  = 0x2012;		/*!< SCO Link scoket option */
const TUint KSolBtESCO 	  = 0x2013;		/*!< eSCO Link scoket option */


/** Synchronous SAP options.
*/
enum TBTSyncOptions
	{
	ESCOSetUserPacketTypes,
	ESyncUserPacketTypes,						/*!< Set user required packet types >*/
	EeSCOExtOptions,							/*!< Set eSCO options >*/
	};


/** SCO socket type
*/
static const TUint KSockBluetoothTypeSCO = KMaxTUint-1;

/** eSCO socket type
*/
static const TUint KSockBluetoothTypeESCO = KMaxTUint-4;
	
const TInt KAirCodingFormatBits = 0x0003;

/** Size of supported commands
 */
const TInt KBluetoothHciCommandsBytes = 64;

/** Bit positions of page 1 of the remote extended features.
 */
enum TBTExtendedFeaturesBitsPage1
	{
	ESecureSimplePairingHostSupportBit          = 0
	};


/**
Handle the local supported hci commands bitmask.  Provides a utility
function to extract individual bits.
*/
NONSHARABLE_CLASS(TBluetoothHciCommands)
	{
public:
	explicit TBluetoothHciCommands();
	explicit TBluetoothHciCommands(const TDesC8& aCommands);
	TBool operator[](TInt aOffset) const;

private:
	void Init(const TDesC8& aCommands);
	
private:
	TBuf8<KBluetoothHciCommandsBytes> iCommands;
	};

/**
An extended double queue link class to provide additional features.
*/
NONSHARABLE_CLASS(TBTDblQueLink) : public TDblQueLink
	{
	public:
		inline TBool IsQueued() const;
	};

/**
Indicates whether the queue link is attached to a queue.
@return True if the link is queued, false otherwise.
*/
inline TBool TBTDblQueLink::IsQueued() const
	{
	return iNext ? ETrue : EFalse;
	}

#endif	// INTERNALTYPES_H
