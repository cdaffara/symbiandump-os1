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
 @publishedAll
 @released
*/

#ifndef PANERR_H
#define PANERR_H

#include <e32base.h>

/**
PAN agent error codes
@note -6551 to -6574 reserved for PAN profile implementation
@note Bluetooth PAN profile error codes
*/
/*@{*/
const TInt KErrPanAgentBase = -6551;

/**
The role state machine did not select a valid pair of roles
*/
const TInt KErrLocalRoleNotSelected = KErrPanAgentBase;					// -6551

/**
Indicates that the roles selected require a baseband role switch
*/
const TInt KErrWaitingForBasebandRoleSwitch = KErrPanAgentBase - 1; 	// -6552

/**
A PAN profile role (U, GN, NAP) has changed from the requested value.
This may require renegotiation of roles, a disconnection of the device, or may just be part of 
normal operation (eg. selecting an actual role when EPanRoleUnknown was provided as an argument)
*/
const TInt KErrRoleChanged = KErrPanAgentBase - 2;						// -6553

/**
Could not set the Class of Device (CoD) when starting the PAN agent
*/
const TInt KErrCouldNotSetCoDDuringStartup = KErrPanAgentBase - 3; 		// -6554

/**
The local device has requested a disconnect
*/
const TInt KErrLocallyInitiatedDisconnect = KErrPanAgentBase - 4;		// -6555

/**
The remote device sent an invalid destination service UUID
*/
const TInt KErrInvalidDestinationServiceUuid = KErrPanAgentBase - 5;	// -6556

/**
The remote device sent an invalid source service UUID
*/
const TInt KErrInvalidSourceServiceUuid = KErrPanAgentBase - 6;			// -6557

/**
Could not role switch to become piconet master
*/
const TInt KErrCouldNotBecomePiconetMaster = KErrPanAgentBase - 7;		// -6558

/**
The remote device sent us a invalid role combination, or requested roles that we cannot fulfil
due to our current state
*/
const TInt KErrInvalidOrUnacceptableRoleCombination = KErrPanAgentBase - 8; // -6559

/**
The remote device did not respond to our role request messages
*/
const TInt KErrRemoteDeviceFailedToRespondToRoleRequests = KErrPanAgentBase - 9;	// -6560

/**
The remote device sent us the wrong sort of packet (eg. a response when we expected a request)
*/
const TInt KErrDodgyResponseFromRemoteDevice = KErrPanAgentBase - 10;	// -6561

/**
No more devices are connected
*/
const TInt KErrAllDevicesDisconnected = KErrPanAgentBase - 11;			// -6562

/**
An attempt has been made to start a connection with incoming support only (despite the 
settings for outgoing connections in commdb), but the connection does not have incoming
support enabled in commdb
*/
const TInt KErrListenForIncomingConnectionRequestedWithoutListeningSupport = KErrPanAgentBase - 12; // -6563

/** 
The settings in commdb have NAP set as the fixed local role
*/
const TInt KErrNapNotSupportedAsLocalRole = KErrPanAgentBase - 13; 		// -6564

/**
The packet driver has not returned us the control handle, and therefore cannot be bnep.drv.
NB : THIS ERROR CODE IS NOT USED
*/
const TInt KErrIncompatiblePacketDriver = KErrPanAgentBase - 14;		// -6565

/**
The specified Fixed Remote Role isn't supported by remote according to SDP query result.
*/
const TInt KErrFixRemoteRoleIsNotSupportedByRemote = KErrPanAgentBase - 15;		// -6566

/*@}*/

#endif // PANERR_H
