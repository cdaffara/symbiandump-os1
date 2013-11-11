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

#ifndef PANCOMMON_H
#define PANCOMMON_H

/**
@file
@note Common definitions that need to be shared between BNEP, PAN agent, and PAN SDP helper
@internalTechnology
*/

#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/nifprvar.h>
#include <networking/panroles.h>
#include <bluetooth/pan/bneppacketnotifier.h>

enum TRemoteDeviceState
/**
A distilled version of the current connection status of the remote device
@internalComponent
*/
	{
	EIdle,						//< Device is Idle or starting up
	EPerformingNegotiation,		//< Device is performing PAN role or M/S role switch.
	EPendingNegotiation,		//< Device is waiting to perfrom a new connection or role change
	EActive,					//< Negotiation is complete and the device is attached to the network
	EDisconnecting,				//< The device is disconnecting
	};
		
enum TBnepSetupConnectionResponseMessage
/**
Response to BNEP connection setup messages.
Defined here so the agent can also see them and use them to respond
@internalComponent
*/
	{
	EOperationSuccessful = 0x00,
	EInvalidDestinationServiceUuid = 0x01,
	EInvalidSourceServiceUuid = 0x02,
	EInvalidServiceUuidSize = 0x03,
	EConnectionNotAllowed = 0x04
	};

enum TBnepAgentToNifNotification
/**
Messages sent from the Agent to the NIF through CBnepLocalDevice::Notify.
@internalTechnology
@released
*/
	{
	/**
	 * Get a pointer to the BNEP core controller.
	 * @see TBnepConnectionManager
	 */
	EPanAgentToNifGetConnectionManager = KVendorSpecificNotificationStart,
	EPanAgentToNifGetPanProvisionInfo,

	/**
	 * Create and return a pointer to the BNEP test API.
	 */
	EPanGetTestApi,
	};

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class MBnepConnectionManager;
NONSHARABLE_CLASS(TBnepConnectionManager)
/**
Encapsulation for a pointer to the central connection manager object.

@see TBnepAgentToNifNotification
@see CBnepLocalDevice::Notification
@internalTechnology
@released
*/
	{
public:
	MBnepConnectionManager* iBnepConnectionManager;
	};
#endif
// 	SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


const TUint KBnepPsm = 0x0f; 		//< The L2CAP PSM for BNEP
const TUint16 KBnepMtu = 1691;		//< The lowest allowable L2CAP MTU for BNEP
const TUint KMaxPanConnections = 8; //< Includes 7 remote devices and the local device

class RInternalSocket;
class MPanConnectionNotify;
class MBnepChannelControl;

class MBnepConnectionManager
/**
"Downwards" agent interface to bnep packet driver core control class
Responsible for passing new connections across and distributing role switching requests to all attached BNEP channels
@internalTechnology
@released
*/
	{
	public:
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        virtual void AttemptNewBnepConnection(const Messages::TNodeId& aSender, const Messages::TSignatureBase& aMessage, TUint16 aActivityId) = 0;
#else        
		/**
		Create a new BNEP channel to the specified remote device
		@param aConnectedSocket An L2CAP socket connected to the remote device
		@param aNotify The object to be informed when events occur on this BNEP channel
		@return A handle to the new BNEP channel object
		*/
		virtual MBnepChannelControl& NewBnepConnectionL(RInternalSocket& aConnectedSocket, MPanConnectionNotify& aNotify) = 0;
#endif

		/**
		Set a flag within BNEP that we are going to retry the connection attempt, so BNEP will
		not signal nifman that we have disconnected
		*/		
		virtual void SetRetryConnect () = 0;

	private:
	public:
	private:
	};

class MBnepChannelControl
/**
"Downwards" control channel between management object for individual BNEP channels in the agent, and their counterparts in the nif (erm, packet driver)
@internalTechnology
@released
*/
	{
	public:
	/**
	A request from the local device for a given set of roles
	@param aRequestedLocalRole The requested role for the local device
	@param aRequestedRemoteRole The requested role for the remote device
	*/
	virtual void BnepRoleRequestFromLocalDevice(TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole) = 0;

	/**
	A response from the local device to the remote device's request for a given set of roles
	@param aRoleResponseCode A code indicating the response to the remote device's request
	*/
	virtual void BnepRoleResponseFromLocalDevice(TBnepSetupConnectionResponseMessage aRoleResponseCode) = 0;
	
	/**
	Disconnect the remote device, and assume this handle will not be used any more
	(The user of this class should drop its reference to this class once Release() has been called)
	@note It should be safe to call the method at any time, even if the remote device has already disconnected, in which case the method should just perform any local cleanup that it needs to
	*/
	virtual void Close() = 0;

	/**
	Method to control if a remote device has access to the uplink when connected to the local PAN NAP service.
	*/
	virtual void SetUplinkAccessAllowedForBnepLink(TBool aAllowed) = 0;
	};

class TUUID;
class MPanConnectionNotify
/**
Upcalls from individual BNEP links to their corresponding management object in the PAN agent
@internalTechnology
@released
*/
	{
	public:
	/**
	A request from a remote device to switch to a specific role
	@note This happens when any BNEP connect packets are received
	@param aRequestedLocalRole The role that the remote device has requested that we assume
	@param aRequestedRemoteRole The role that the remote device has requested for itself
	*/
	virtual void BnepRoleRequestFromRemoteDevice(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole) = 0;
	
	/**
	A response from a remote device indicating the result of an attempt by us to perform a BNEP connection to it
	@param aRoleResponseCode A response from the remote device indicating the result of the BNEP connection attempt
	*/
	virtual void BnepRoleResponseFromRemoteDevice(TBnepSetupConnectionResponseMessage aRoleResponseCode) = 0;
	
	/**
	Indication that the remote device has disconnected
	@param aError An error code indicating why the remote device disconnected
	*/
	virtual void RemoteDeviceDisconnect(TInt aError) = 0;
			
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	virtual MPanLinkControlBase& BnepConnectionController() = 0;
#endif
	};

#endif // PANCOMMON_H
