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

#ifndef HCTLCHANNELOBSERVER_H
#define HCTLCHANNELOBSERVER_H

#include <e32base.h>

/** Definition of the HCI transport channel type */
typedef TUint16 THCITransportChannel;	

/** No HCI transport channels available for sending */
static const THCITransportChannel KHCITransportNoChannels		= 0x0000;

/** Command HCI transport channel available for sending */
static const THCITransportChannel KHCITransportCommandChannel	= 0x0001;

/** ACL data HCI transport channel available for sending */
static const THCITransportChannel KHCITransportACLDataChannel	= 0x0002;

/** SCO data HCI transport channel available for sending */
static const THCITransportChannel KHCITransportSCODataChannel	= 0x0004;

/** All HCI transport channels available for sending */
static const THCITransportChannel KHCITransportAllChannels = static_cast<THCITransportChannel>(KHCITransportCommandChannel | 
																							   KHCITransportACLDataChannel | 
																							   KHCITransportSCODataChannel);

/** The UID associated with the MHCTLChannelObserver interface. */
const TInt KHCTLChannelObserverUid = 0x102736EF;

/**
API for observing information regarding the status of a HCTL channel.

This should be implemented by the stack so it can control the multiplexing
of the command and data sent over the HCTL.
*/
class MHCTLChannelObserver
	{
public:
	/**
	Indication that a HCTL channel has been opened.

	@param aChannel A bitmask containing the channels which are now open.
	*/
	virtual void MhcoChannelOpen(THCITransportChannel aChannels) =0;

	/**
	Indication that a HCTL channel has been closed.

	@param aChannel A bitmask containing the channels which are now closed.
	*/
	virtual void MhcoChannelClosed(THCITransportChannel aChannels) =0;
	};

#endif // HCTLCHANNELOBSERVER_H
