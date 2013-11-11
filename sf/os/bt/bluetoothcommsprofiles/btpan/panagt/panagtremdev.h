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

#ifndef PANAGTREMDEV_H
#define PANAGTREMDEV_H

/**
@file
@note PAN agent remote device class definition, and remote device state base class definitions
@internalComponent
*/

#include "panagt.h"
#include "panhelper.h"
#include "panagtutils.h"

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
#include "panmessages.h"
#endif

class MBnepChannelControl;
class TPanConnectionList;



namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

/**
The number of role negotiation cycles we are willing to go through before terminating the 
connection to the remote device
@internalComponent
*/
const TUint KMaxBnepNegotiationCyles = 3;

/**
The amount of time we will wait for a role response before resending the role request
@internalComponent
*/
const TUint KMaxTimeToWaitForRoleResponse = 10*1000*1000; // 10 secs (from PAN spec v1.0)

/**
The maximum amount of time we will wait for a master/slave switch to occur
@internalComponent
*/
const TUint KMaxTimeToWaitForBasebandRoleSwitch = 5*1000*1000;	// 5 secs

class MPanConnectionNotifyForState
/**
A mirror class of MPanConnectionNotify, but with leaving versions of the methods to allow easier 
error handling for each state. This class should be kept in sync with the methods in MPanConnectionNofify.
@see MPanConnectionNotify
*/
	{
public:
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole) = 0;
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode) = 0;
	virtual void RemoteDeviceDisconnectL(TInt aError) = 0;
	};

class MPanRemDevStateMachineNotify;
NONSHARABLE_CLASS(CPanRemDevStateBase) : public CBase, public MPanConnectionNotifyForState
/**
Base class for remote device states
@internalComponent
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    friend class CPanRemoteDeviceStateMachine;
#endif

protected:
	enum TPanRemDevStates
		{
		EPanRemDevStatePerformingSdpQuery,
		EPanRemDevStateConnectingSocket,
		EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection,
		EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection,
		EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest,
		// EPanRemDevStatePerformingRoleNegotiationForOutgoingBase,
		EPanRemDevStatePerformingRoleNegotiationForOutgoingConnection,
		EPanRemDevStatePerformingRoleNegotiationForIncomingConnection,
		EPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange,
		EPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection,
		// EPanRemDevStateWaitingForRoleSwitchBase,
		EPanRemDevStateWaitingForRoleSwitchForOutgoingConnection,
		EPanRemDevStateWaitingForRoleSwitchForIncomingConnection,
		EPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest,
		EPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest,
		EPanRemDevStateActive,
		EPanRemDevStateShutdown,
		EPanRemDevStateIncomingNotification,
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		EPanRemDevStatePaused,
#endif		
		};

public:
	virtual ~CPanRemDevStateBase();
	virtual void OnEntryL() = 0;
	
	// MPanConnectionNotifyForState methods (downcalls from state machine)
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	//
	// Other downcalls from state machine
	//
	/**
	Called from the state machine when it's ready to negotiate roles for this connection
	@note This can be called when the connection is active, and indicates that the state machine wants to renegotiate roles
	@note Clients must set a flag and initiate a callback to start the RoleRequest() process to prevent call stacks from getting too large
	@leave To indicate that an unrecoverable error has occured, and that the connection should be shut down
	*/
	virtual void ReadyForRoleRequestL();
	
	/**
	A locally-initiated disconnect
	*/
	virtual void ShutdownL() = 0;

	virtual TRemoteDeviceState GetState() const = 0;
	
	// Async translation calls - needed to convert an async event from signalling a TRequestStatus into an async event as a function call on this object
	virtual void AsyncEventCompleteL();
	virtual void AsyncEventCancelL();

protected:
	CPanRemDevStateBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);

	void ConvertUuidsToPanRolesL(const TUUID& aLocalRoleUuid, const TUUID& aRemoteRoleUuid, TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole);
	TInt PerformMasterSlaveSwitchIfNecessary(TBluetoothPanRole aLocalRole);

	MPanRemDevStateMachineNotify& StateMachine();
	
	void PanicInState(TPanAgentPanic aPanic) const;

private:
	TInt UuidToPanRole(const TUUID& aUuid, TBluetoothPanRole& aRole);
	TInt BecomePiconetMaster();

public:

private:
	MPanRemDevStateMachineNotify& iStateMachine;
	TPanRemDevStates iStateNumber;
	};

class MPanRemDevStateMachineNotify
/**
Upcalls from states to state machine
@internalComponent
*/
	{
public:
	//
	// Accessor methods
	//
	/**
	Get a handle to the local role
	*/
	virtual TBluetoothPanRole& LocalRole() = 0;
	
	/**
	Get a handle to the remote role
	*/
	virtual TBluetoothPanRole& RemoteRole() = 0;
	
	/**
	Get handle to PAN remote SDP querier
	*/
	virtual RPanRemoteSdpQuerier& RemoteSdpQuerier() = 0;

	/**
	Get role array
	*/
	virtual TPanDeviceRolesList& RemoteRolesList() = 0;
	
	/**
	Get WorthTrying role array
	*/
	virtual TPanDeviceWorthTryingRolesList& RemoteWorthTryingRolesList() = 0;
	
	/**
	Get the remote device address
	*/
	virtual TBTSockAddr& RemSockAddr() = 0;
	
	/**
	Get a handle to the socket
	*/
	virtual RInternalSocket& Socket() = 0;
	
	/**
	Get a handle to commdb
	*/
	virtual CCommsDbAccess& Database() = 0;
	
	/**
	Get hold of the state machine's TRequestStatus either to use it for an async call, or to check its value after an event has completed
	@return The state machine's TRequestStatus
	*/
	virtual TRequestStatus& Status() = 0;

	/**
	Are SDP queries disabled by the settings in commdb?
	*/
	virtual void SdpQueriesDisabled(TBool aSdpQueriesStatus) = 0;
	virtual TBool IsSdpQueriesDisabled() = 0;
	
	/**
	Get the error that occured (for shutdown state)
	*/
	virtual TInt Error() const = 0;
	
	/**
	Have we got to a stage where we have a BNEP channel open?
	*/
	virtual TBool HasBnepChannel() const = 0;
	
	//
	// do'er methods
	//
	/**
	Make the parent object active
	*/
	virtual void SetActive() = 0;
	
	/**
	Cancel the pending AO
	@note It is the caller's responsibilty to ensure that whichever event it was waiting for is also cancelled
	*/
	virtual void Cancel() = 0;
	
	/**
	Inform the PAN agent role state machine that we are now active
	*/
	virtual void DeviceActive() = 0;
	
	/**
	Inform the PAN agent role state machine that the role upgrade failed
	*/
	virtual void RoleChangeFailed() = 0;
	
	/**
	Inform the PAN agent role state machine that our shutdown is complete
	*/
	virtual void ShutdownComplete() = 0;
	
	/**
	Call to initiate an outgoing PAN connection from the remote device state machine.
	@param aLocalRole The local role will be decided by the PAN agent state machine logic.  
	                  No value should be passed into this method call for this parameter.
	                  When this method returns with KErrNone this value will contain the local role to connect to.
	@param aRemoteRole The remote role will be decided by the PAN agent state machine logic.
	                  No value should be passed into this method call for this parameter
	                  When this method returns with KErrNone this value will contain the remote role to connect to.
	@param aWorthTryingRemoteRoles This is a list of the remote PAN roles that are worth trying to connect to this device.
	@return A system wide or PAN agnet specific error code indicating if the connection should continue
	*/
	virtual TInt InitiateOutgoingConnection(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles) = 0;

	/**
	Call to initiate an outgoing role change with an existing remote device from the remote device state machine.
	@param aLocalRole The local role will be decided by the PAN agent state machine logic.  
	                  No value should be passed into this method call for this parameter.
	                  When this method returns with KErrNone this value will contain the local role to connect to.
	@param aRemoteRole The remote role will be decided by the PAN agent state machine logic.
	                  No value should be passed into this method call for this parameter
	                  When this method returns with KErrNone this value will contain the remote role to connect to.
	@return A system wide or PAN agnet specific error code indicating if the role change procedure should continue
	*/
	virtual TInt PerformLocalRoleChangeRequest(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole) = 0;

	/**
	Called from the remote device state machine when it has received a role change request from the peer device.
	@param aLocalRole The requested local role from the peer BNEP Setup Request.  
	@param aRemoteRole The requested remote role from the peer BNEP Setup Request.  
	@return A system wide or PAN agnet specific error code indicating if the role change procedure should continue.
	*/
	virtual TInt RoleChangeRequestFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole) = 0;

	/**
	Called from the remote device state machine when it has received a incoming connection request from a peer device.
	@param aLocalRole The requested local role from the peer BNEP Setup Request.  
	@param aRemoteRole The requested remote role from the peer BNEP Setup Request.  
	@return A system wide or PAN agnet specific error code indicating if the incoming connection request should be accepted.
	*/
	virtual TInt IncomingConnectionFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole) = 0;
	
	/**
	Send a role request to the remote device
	@param aLocalRole The proposed local role
	@param aRemoteRole The proposed remote role
	*/
	virtual void SendRoleRequest(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole) = 0;
	
	/**
	Send a role reponse to the remote device
	@param aRoleReponseCode A BNEP error code indicating the status of the request
	*/
	virtual void SendRoleResponse(TBnepSetupConnectionResponseMessage aRoleResponseCode) = 0;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	/**
	Create a new BNEP channel
	@param aConnectedSocket An L2CAP socket connected to the remote device
	@param aActivity the activity to be used when sending the creation request to Bnep
	*/
	virtual void CreateNewBnepConnection(RInternalSocket& aConnectedSocket, TPanMessage::TPanActivity aActivity) = 0;

#else
	/**
	Create a new BNEP channel
	@param aConnectedSocket An L2CAP socket connected to the remote device
	*/
	virtual void CreateNewBnepConnectionL(RInternalSocket& aConnectedSocket) = 0;
#endif

	/**
	Disconnect the BNEP channel.  This should disconnect the remote remote device if it is connected.
	@note This is safe to call at any point - if there is no BNEP channel at the moment, the implementation should just do nothing
	*/
	virtual void DisconnectBnepChannel() = 0;

	/**
	Specify if uplink access is allowed for this remote device
	*/
	virtual void SetUplinkAccessAllowed(TBool aAllowed) = 0;

	/**
	Read if uplink access is allowed for this remote device
	*/
	virtual TBool UplinkAccessAllowed() const = 0;
		
	/**
	Put the state machine into a new state
	@param aState The new state
	*/
	virtual void SetState(CPanRemDevStateBase& aState) = 0;
	
	/**
	Get a handle to physical link adapter
	*/
	virtual RBTPhysicalLinkAdapter& PhysicalLinkAdapter() = 0;
	
	/**
	Initialise physical link adapter
	*/
	virtual void OpenPhysicalLinkAdapterL() = 0;

	virtual void SetRetryConnect() = 0;

	virtual TInt IncrementConnectionRetryAttempts() = 0;
	virtual void ResetConnectionRetryAttempts() = 0;
	
	virtual void ResetRetryParameters() = 0;
	virtual TBool WorthTrying() = 0;
	
	/**
	Populate the PAN connection list provided as an argument
	@param A reference to the PAN connection list contained that should be populated.
	*/	
	virtual void GetExistingConnections(TPanConnectionList& aPanConnectionList) = 0;
	virtual void Shutdown(TInt aError) = 0;
protected:
	virtual ~MPanRemDevStateMachineNotify() {};
	};
	
NONSHARABLE_CLASS(CPanRemoteDeviceStateMachine) :
    public CActive,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    public Messages::ASimpleNodeIdBase,
#endif
    private MPanConnectionNotify,
    private MPanRemDevStateMachineNotify
/**
Management of a connection to a remote device
@note This has a 1-1 relationship with BNEP channels in the bnep packet driver
@internalComponent
*/
	{
public:
	static CPanRemoteDeviceStateMachine* NewL(RInternalSocket& aConnectedSocket, MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase);
	static CPanRemoteDeviceStateMachine* NewL(const TBTDevAddr& aRemDevAddr, MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase);
	~CPanRemoteDeviceStateMachine();

	// MPanConnectionNotify methods	
	virtual void BnepRoleRequestFromRemoteDevice(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDevice(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnect(TInt aError);

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	virtual MPanLinkControlBase& BnepConnectionController();
#endif

	// Downcalls from pan agent state machine
	void ReadyForRoleRequest();
	void Disconnect();
	TInt DisallowRoleSwitch();
	TInt AllowRoleSwitch();
	TBTDevAddr DevAddr() const;

	// MPanAgtRemDevStateMachineNotify methods (upcalls from states)
	virtual TBluetoothPanRole& LocalRole();
	virtual TBluetoothPanRole& RemoteRole();
	virtual RPanRemoteSdpQuerier& RemoteSdpQuerier();
	virtual TPanDeviceRolesList& RemoteRolesList();
	virtual TPanDeviceWorthTryingRolesList& RemoteWorthTryingRolesList();
	virtual TBTSockAddr& RemSockAddr();	
	virtual RInternalSocket& Socket();
	virtual CCommsDbAccess& Database();
	virtual TRequestStatus& Status();
	virtual void SdpQueriesDisabled(TBool aSdpQueriesStatus);
	virtual TBool IsSdpQueriesDisabled();
	virtual TInt Error() const;
	virtual TBool HasBnepChannel() const;
	virtual void SetActive();
	virtual void Cancel();
	virtual void DeviceActive();
	virtual void RoleChangeFailed();
	virtual void ShutdownComplete();

	virtual TInt InitiateOutgoingConnection(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual TInt PerformLocalRoleChangeRequest(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole);
	virtual TInt RoleChangeRequestFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole);
	virtual TInt IncomingConnectionFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole);

	virtual void SendRoleRequest(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole);
	virtual void SendRoleResponse(TBnepSetupConnectionResponseMessage aRoleResponseCode);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	virtual void CreateNewBnepConnection(RInternalSocket& aConnectedSocket, TPanMessage::TPanActivity aActivity);
#else
	virtual void CreateNewBnepConnectionL(RInternalSocket& aConnectedSocket);
#endif

	virtual void DisconnectBnepChannel();
	virtual void SetUplinkAccessAllowed(TBool aAllowed);
	virtual TBool UplinkAccessAllowed() const;
	virtual void SetState(CPanRemDevStateBase& aState);
	virtual RBTPhysicalLinkAdapter& PhysicalLinkAdapter();
	virtual void OpenPhysicalLinkAdapterL();
	virtual void SetRetryConnect();
	virtual TInt IncrementConnectionRetryAttempts();
	virtual void ResetConnectionRetryAttempts();
	virtual void ResetRetryParameters();
	virtual TBool WorthTrying();
	
	virtual TRemoteDeviceState GetState() const;

	virtual TBool NewConnectedDeviceProgressSent() const;
	virtual void SetNewConnectedDeviceProgressSent(TBool aNewConnectedDeviceProgressSent);

	virtual void GetExistingConnections(TPanConnectionList& aPanConnectionList);
	virtual void Shutdown(TInt aError);

	// CActive methods
	virtual void RunL();
	virtual void DoCancel();
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
protected:
    // From ESock::MCFNode, via ESock::ACFNodeIdBase
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);
#endif

private:
	CPanRemoteDeviceStateMachine(MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase);
	void ConstructL(const TBTDevAddr& aRemDevAddr);
	void ConstructL(RInternalSocket& aConnectedSocket);
		
public:

private:
	TBTSockAddr iRemSockAddr;  		//< needed for baseband role switching, and SDP queries
	TBool iSdpQueriesDisabled;		//< Are SDP queries disabled by the settings in commdb?

	TBluetoothPanRole iLocalRole;	//< the local role (or proposed local role) for this connection
	TBluetoothPanRole iRemoteRole;	//< the remote role (or proposed remote role) for this connection
	TPanDeviceRolesList iRemotePanRolesFromSdp; //< The roles that the remote device is advertising, discovered using SDP
	TPanDeviceWorthTryingRolesList iRemoteWorthTryingRoles; // The roles that are worth trying with the remote device

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY									
	Messages::TNodeId iBnepChannelController; 
#else
	MBnepChannelControl* iBnepChannelController; //< interface to channel in bnep packet driver
#endif
	
	MRemoteDeviceNotify& iParent; 	//< The class to be notified when events occur
	CCommsDbAccess& iDatabase;		//< Handle to commdb to allow us to read it (note that reading through this handle also takes account of an overrides that might be set)
	
	RInternalSocket iSocket;		//< Socket used for creating new outgoing connections, or as a quick place to stash the (connected) incoming socket whilst we set up the initial state
									// also used later as the socket to communicate with the proxy SAP
	TInt iError;					//< Somewhere to stash any error that occurs for access by shutdown state
	
	CPanRemDevStateBase* iState; 	//< Our current state
	CPanRemDevStateBase* iShutdownState; //< Preallocate to ensure we can always shut down a connection

	RPanRemoteSdpQuerier iRemoteSdpQuerier;	//< Remote SDP querier
	RSocketServ iSockServ;
	RBTPhysicalLinkAdapter iPhysicalLinkAdapter;
	
	TInt iConnectionRetryAttempts;
	
	TBool iUplinkAccessAllowed;				//< Indicates if the remote device is allowed access to the uplink
	TBool iNewConnectedDeviceProgressSent;	//< Indicates if a new connected device notification has been sent from the agent to the RConnection client for this remote device
	};
	
}	// PanAgent
#endif // PANAGTREMDEV_H
