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

#ifndef PANAGT_H
#define PANAGT_H

/**
@file
@note PAN agent agent class definitions and global constants
@internalComponent
*/

#include <comms-infras/cagentbase.h>
#include <comms-infras/eintsock.h>
#include <panprog.h>
#include <panerr.h>
#include <bt_sock.h>
#include "pancommon.h"
#include "panagtincoming.h"
#include "panagtremdevselector.h"
#include "panagtutils.h"
#include "pancoexistenceconnectioncontroller.h"

using namespace PanHelper;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_node.h>
#include <comms-infras/ss_common.h>
class CPanProvisionInfo;
#endif

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

const TUint KMajorVersionNumber = 1;
const TUint KMinorVersionNumber = 0;
const TUint KBuildVersionNumber = 0;

/** 
Priority of active objects in the PAN agent (mainly the state machine(s) and callbacks)
@internalComponent
@released
@see KPanAgtAoRemoteDevicePromptPriority
*/
const TInt KPanAgtAoPriority = CActive::EPriorityStandard; 

const TUint KPanAgtConnectionRetryAttempts = 1;

NONSHARABLE_CLASS(CPanAgentFactory) : public CNifAgentFactory
/**
Create instances of the PAN agent
@internalComponent
*/
	{
protected:
	void InstallL();
	CNifAgentBase* NewAgentL(const TDesC& aName);
	TInt Info(TNifAgentInfo& aInfo, TInt aIndex) const;
	};

//
// Connection management objects
//

//
// Interface for upcalls from remote device management objects
//
class CPanRemoteDeviceStateMachine;

class MRemoteDeviceNotify
/**
Upcalls from CPanRemoteDeviceStateMachine
@internalComponent
*/
	{
public:
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
	PAN role with the remote device has completed successfully
	@pre This should follow a call to RoleRequest
	@param aNewDevice The class managing the device which is now active in the PAN
	*/
	virtual void DeviceActive(CPanRemoteDeviceStateMachine& aNewDevice) = 0;
	
	/**
	The attempt to switch roles failed
	@note This should only be called by devices marked as negotiating
	@param aDevice The class managing the device which completed the remote connection
	*/
	virtual void DeviceRoleChangeFailed(CPanRemoteDeviceStateMachine& aDevice) = 0;

	/**
	BNEP level connection to the remote device has been closed, and all necessary cleanup has been performed
	@note This can occur because we couldn't negotiate mutually satifactory PAN roles, or because the device has disconnected (or we disconnected it)
	@note This will delete the remote device object from the device array
	@param aDevice The class managing the device which completed the remote connection
	*/
	virtual void DeviceDisconnected(CPanRemoteDeviceStateMachine& aDevice) = 0;
	


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	virtual const Messages::TNodeId& BnepConnectionManager() = 0;
	
	virtual void RestartIncomingConnectionListener(TInt aError) = 0;
#else

	/**
	Get a handle to the BNEP connection manager
	@return A reference to the BNEP connection manager
	*/
	virtual MBnepConnectionManager& BnepConnectionManager() = 0;
	
	/**
	Get a handle to the BNEP link controller
	@return A reference to the BNEP link controller
	*/
	virtual MPanLinkControlBase& BnepConnectionController() = 0;
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
	
	/**
	Retry the connection to the specified device.
	@param A reference to the remote device that should be reconnected to.
	*/
	virtual void SetRetryConnect(CPanRemoteDeviceStateMachine& aDevice) = 0;

	/**
	Populate the PAN connection list provided as an argument
	@param A reference to the PAN connection list contained that should be populated.
	*/	
	virtual void GetExistingConnections(TPanConnectionList& aPanConnectionList) = 0;

	/**
	Close existing connection (there can be only zero or one) that has access to the uplink.
	THIS SHOULD BE REMOVED WHEN >1 REMOTE DEVICE IS ALLOWED TO ACCESS THE UPLINK.
	*/
	virtual void CloseExistingConnectionWithUplinkAccess() = 0;
	
	/**
	Check if it is worth trying the connection. If we have a fixed role and this role is not worth trying return EFalse.
	@param remote worth trying roles.
	*/
	virtual TBool WorthTrying(const TPanDeviceWorthTryingRolesList& aRemoteWorthTryingRoles) const= 0;

	
	
protected:
	virtual ~MRemoteDeviceNotify()	{};
	};	


//
// Pan agent state machine base classes
//

class MRemoteDeviceNotifyForState
/**
A mirror class of MRemoteDeviceNotify, but with leaving versions of the methods to allow easier 
error handling for each state. This class should be kept in sync with the methods in MRemoteDeviceNotify,
except for simple getter methods, such as BnepConnectionManager, which are merely to allow sharing of
handles between the remote device and role state machines.
@see MRemoteDeviceNotify
@internalComponent
*/
	{
public:
	/**
	@see MRemoteDeviceNotify::InitiateOutgoingConnectionL
	*/
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles) = 0;

	/**
	@see MRemoteDeviceNotify::PerformLocalRoleChangeRequestL
	*/
	virtual void PerformLocalRoleChangeRequestL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole) = 0;

	/**
	@see MRemoteDeviceNotify::RoleChangeRequestFromPeerL
	*/
	virtual void RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole) = 0;

	/**
	@see MRemoteDeviceNotify::IncomingConnectionFromPeerL
	*/
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole) = 0;

	/**
	@see MRemoteDeviceNotify::DeviceActive
	@note Leaving will cause the connection to be shut down
	*/
	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aNewDevice) = 0;

	/**
	@see MRemoteDeviceNotify::DeviceRoleChangeFailed
	@note Leaving will cause the connection to be shut down
	*/
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice) = 0;

	/**
	@see MRemoteDeviceNotify::DeviceDisconnected
	@note Leaving will cause the connection to be shut down
	*/
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice) = 0;	
	
protected:
	virtual ~MRemoteDeviceNotifyForState()	{};
	};

class MPanAgtStateMachineNotify;
NONSHARABLE_CLASS(CPanAgtStateBase) : public CBase, public MRemoteDeviceNotifyForState
/**
Base class for PAN agent states
@internalComponent
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    friend class CPanAgent;
#endif

protected:
	enum TPanAgtStates
		{
		EPanAgentStateUninitialised,
		EPanAgentStateInitialising,
		EPanAgtStateIdle,
		EPanAgtStateNegotiatingURole,
		EPanAgtStateURole,
		EPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest,
		EPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest,
		EPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest,
		EPanAgtStateNegotiatingGnOrNapRole,
		EPanAgtStateGnOrNapRole,
		EPanAgtStateShutdown,
		EPanAgtStateReconnect,
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		EPanAgtStatePaused,
#endif
		};
public:
	~CPanAgtStateBase();
	// MRemoteDeviceNotify methods (downcalls from state machine)
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void PerformLocalRoleChangeRequestL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole);
	virtual void RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

	virtual void StateSendRoleNotification(); // overridden for states EPanAgtStateURole and EPanAgtStateGnRole
	
	/**
	Call to make a new state "active" - called when the state is set as the active state in the state machine
	@note All states should perform initialisation on this call
	*/
	virtual void OnEntryL() = 0;
	virtual void Connect();
	virtual void CancelConnect();
	virtual void CancelReconnect();

	TInt IsRoleRequestValid(TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole) const;

protected:
	CPanAgtStateBase(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber);
	
	void BaseConstructL();
	void StartNegotiationWithFirstPendingDevice();

	// methods for standard operations common to many/all states
	void DoDeviceDisconnectL(CPanRemoteDeviceStateMachine& aDevice);

	// methods for negotiation
	TInt SelectRemoteRoleBasedOnLocalRole(TBluetoothPanRole aSelectedLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);

	TInt FindDeviceInArray(const CPanRemoteDeviceStateMachine& aDevice) const;
	TInt DeviceCountInState(TRemoteDeviceState aState) const;

	// methods for making sure GN and NAP work correctly!
	void LockDeviceAsPiconetMasterL();
	TInt ReleasePiconetMasterLock();

	// methods for ASSERTs
	TInt NegotiatingDeviceCount() const;
	TInt ActiveDeviceCount() const;
	
	MPanAgtStateMachineNotify& StateMachine();
	const MPanAgtStateMachineNotify& StateMachine() const;

	// Methods for SDP registration and listener starting/stopping.
	void StartListenerL();
	void StopListener();
	
	void PanicInState(TPanAgentPanic aPanic) const;

	// debugging methods
#ifdef _DEBUG	
	void DumpDeviceArray() const;
#endif // _DEBUG
private:
	static TInt ServiceStartedCb(TAny* aThisPtr);
	static TInt ConnectCompleteCb(TAny* aThisPtr);
	void ServiceStarted();
protected:
	CAsyncCallBack* iServiceStartedCallback;
	CAsyncCallBack* iConnectCompleteCallback;
private:
	MPanAgtStateMachineNotify& iStateMachine;
	TPanAgtStates iStateNumber;
	};

class CPanAgentStateShutdown;
class CPanHelperLocalSdpRegistrar;	
class MPanAgtStateMachineNotify
/**
Upcalls from PAN agent role states to PAN agent role state machine
@internalComponent
*/
	{
	friend class CPanAgtStateShutdown; // to ensure that only the shutdown state can call FullShutdownComplete()
public:
	//
	// Accessor methods
	//
	/**
	Get a handle to the device array
	@return A handle to the device array
	*/
	virtual RPointerArray<CPanRemoteDeviceStateMachine>& Devices() = 0;

	/**
	Get a const handle to the device array
	@return A const handle to the device array
	*/
	virtual const RPointerArray<CPanRemoteDeviceStateMachine>& Devices() const = 0;

	/**
	Get a handle to the fixed remote role field in the state machine
	@note This is not const, so can be used to read or write the field in the state machine
	*/
	virtual TBluetoothPanRole& FixedLocalRole() = 0;

	/**
	Get a handle to the fixed remote role field in the state machine
	@note This is not const, so can be used to read or write the field in the state machine
	*/
	virtual TBluetoothPanRole& FixedRemoteRole() = 0;

	/**
	Check if the agent configured to allow incoming connections
	@return ETrue if incoming connections are allowed 
	*/
	virtual TBool AllowIncoming() const = 0;

	/**
	Set if the agent configured to allow incoming connections
	@param ETrue if incoming connections are allowed 
	*/
	virtual void SetAllowIncoming(TBool aAllowIncoming) = 0;

	/**
	Check if the agent configured to allow connections to the local role of NAP
	@return ETrue if connections to the local role of NAP are allowed 
	*/
	virtual TBool NapEnabled() const = 0;

	/**
	Set if the agent configured to allow connections to the local role of NAP
	@param ETrue if connections to the local role of NAP are allowed 
	*/
	virtual void SetNapEnabled(TBool aNapEnabled) = 0;
	
	/**
	Get the handle to commdb
	@return A handle to the comms database
	*/
	virtual CCommsDbAccess& Database() = 0;

	/**
	Get the handle to the PAN local SDP registrar
	*/
	virtual CPanHelperLocalSdpRegistrar& PanLocalSdpRegistrar() = 0;

	/**
	Return the IAP ID of this connection
	*/
	virtual TInt IapId() = 0;
	
	//
	// Do-er methods
	//
	/**
	Suppress outgoing connections
	*/
	virtual void SetSuppressOutgoing() = 0;

	/**
	Stop outgoing connections being suppressed
	*/
	virtual void ResetSuppressOutgoing() = 0;

	/**
	Send a progress notification to clients via nifman
	*/
	virtual void SendProgress(TInt aProgress, TInt aError) = 0;

	/**
	Start the incoming connection listener
	@return KErrNone if successful, or one of the Symbian standard error codes
	*/
	virtual TInt StartIncomingConnectionListener() = 0;

	/**
	Stop and delete the incoming connection listener
	*/	
	virtual void CloseIncomingConnectionListener() = 0;
	
	/**
	Start the process of setting up any outgoing connections we might need
	*/
	virtual void SetupOutgoingConnectionsL() = 0;

	/**
	Forwarding call to Nifman
	*/
	virtual void ServiceStarted() = 0;
	
	/**
	Forwarding call to Nifman
	*/
	virtual void ConnectComplete() = 0;
	
	/**
	Forwarding call to Nifman
	*/
	virtual void ReconnectComplete(TInt aError) = 0;
	
	//
	// Getter methods
	//
	/**
	Find out whether outgoing connections are suppressed
	*/
	virtual TBool SuppressOutgoing() const = 0;
	
	/**
	Is the incoming connection listener active?
	@return Whether the incoming connection listener is enabled (including in pause mode) or not
	*/
	virtual TBool IsIncomingConnectionListenerEnabled() const = 0;
	
	/**
	Is the user being shown the device selection notifier?
	*/
	virtual TBool IsPromptingUserForDeviceSelection() const = 0;
	
	/**
	Check whether we're allowed to act as a U (according to commdb settings)
	*/
	virtual TBool IsUAllowedByFixedLocalRole() const = 0;
	
	/**
	Check whether we're allowed to act as the GN (according to commdb settings)
	*/
	virtual TBool IsGnAllowedByFixedLocalRole() const = 0;

	/**
	Check whether we're allowed to act as a NAP (according to commdb settings)
	*/
	virtual TBool IsNapAllowedByFixedLocalRole() const = 0;

	//
	// Setter methods
	//
	/**
	Put the state machine into a new state
	@param aState The new state
	*/
	virtual void SetState(CPanAgtStateBase& aState) = 0;
	
	virtual void HandleAllPanConnectionsClosed() = 0; 
	virtual TBool IsNewPanConnectionAllowed() const = 0;
	
protected:
	/**
	Inform the state machine that the shutdown state has finished shutting down the remote connections
	*/
	virtual void FullShutdownComplete() = 0;
	
	/**
	Stop anyone deleting the object through its M-class pointer
	*/
	virtual ~MPanAgtStateMachineNotify(){};
	};

class CPanRemoteDeviceSelector;
class CPanIncomingConnectionListener;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
NONSHARABLE_CLASS(CPanAgent) : public CAgentBase, public Messages::ASimpleNodeIdBase,
    public MPanCoexistenceConnectionObserver, private MNifAgentExtendedManagementInterface,
    private MPanAgtStateMachineNotify, private MRemoteDeviceNotify,
    private MIncomingConnectionAcceptor, private MOutgoingConnectionCreator

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
NONSHARABLE_CLASS(CPanAgent) : public CAgentBase,
    private MNifAgentExtendedManagementInterface,
    private MPanAgtStateMachineNotify, private MRemoteDeviceNotify,
    private MIncomingConnectionAcceptor, private MOutgoingConnectionCreator,
    private MPanLinkControl
#endif
//SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
The core agent class, including PAN role state machine
@internalComponent
*/
	{
public:
	static CPanAgent* NewL();
	~CPanAgent();
	
	// C(Nif)AgentBase methods
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual TInt Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
	virtual void Info(TNifAgentInfo& aInfo) const;
	virtual void Connect(TAgentConnectType aType);
	virtual void Connect(TAgentConnectType aType, CStoreableOverrideSettings* aOverrideSettings);
	virtual void CancelConnect();
	virtual void Reconnect();
	virtual void CancelReconnect();
	virtual void Disconnect(TInt aReason);
	virtual TInt GetExcessData(TDes8& aDes);
	virtual TInt IncomingConnectionReceived();
	virtual void GetLastError(TInt& aError);
	virtual TBool IsReconnect() const;
	
	// MPanCoexistenceConnectionObserver
	virtual void MpccoPanCoexistenceConnectionUpdated(TIPBearerCoexistenceStatus aStatus); 
	
	// MNifAgentExtendedManagementInterface methods
	virtual TInt GetInterfaceType(TConnectionType& aConnectionType);
	virtual TInt EnumerateSubConnections(TUint& aCount);
	virtual TInt GetSubConnectionInfo(TUint aIndex, TDes8& aSubConnectionInfo);
	virtual TInt GetSubConnectionInfo(TDes8& aSubConnectionInfo);
	
	// MPanAgtStateMachineNotify methods
	virtual RPointerArray<CPanRemoteDeviceStateMachine>& Devices();
	virtual const RPointerArray<CPanRemoteDeviceStateMachine>& Devices() const;
	virtual TBluetoothPanRole& FixedLocalRole();
	virtual TBluetoothPanRole& FixedRemoteRole();
	virtual CPanHelperLocalSdpRegistrar& PanLocalSdpRegistrar();	
	virtual CCommsDbAccess& Database();
	virtual TInt IapId();
	virtual TBool SuppressOutgoing() const;
	virtual void SetSuppressOutgoing();
	virtual void ResetSuppressOutgoing();
	virtual void SendProgress(TInt aProgress, TInt aError);
	virtual TBool IsIncomingConnectionListenerEnabled() const;
	virtual TBool IsPromptingUserForDeviceSelection() const;
	virtual TBool IsUAllowedByFixedLocalRole() const;
	virtual TBool IsGnAllowedByFixedLocalRole() const;
	virtual TBool IsNapAllowedByFixedLocalRole() const;
	virtual TInt StartIncomingConnectionListener();
	virtual void CloseIncomingConnectionListener();
	virtual void SetupOutgoingConnectionsL();
	virtual void ServiceStarted();
	virtual void ConnectComplete();
	virtual void ReconnectComplete(TInt aError);
	virtual void SetState(CPanAgtStateBase& aState);
	virtual void FullShutdownComplete();
	
	virtual void HandleAllPanConnectionsClosed();
	virtual TBool IsNewPanConnectionAllowed() const;
	
	// MIncomingConnectionAcceptor methods
	virtual void CreateNewConnectionFromIncomingL(RInternalSocket& aConnectedSocket);

	// MOutgoingConnectionCreator methods
	virtual void CreateNewConnectionForOutgoingL(const TBTDevAddr& aRemDevAddr);
	virtual TInt DeviceSelectionError(TInt aErr);

	// MRemoteDeviceNotify methods
	virtual TInt InitiateOutgoingConnection(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual TInt PerformLocalRoleChangeRequest(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole);
	virtual TInt RoleChangeRequestFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole);
	virtual TInt IncomingConnectionFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole);

	virtual void DeviceActive(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailed(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnected(CPanRemoteDeviceStateMachine& aDevice);


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    virtual const Messages::TNodeId& BnepConnectionManager();
	virtual void RestartIncomingConnectionListener(TInt aError);

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	virtual MBnepConnectionManager& BnepConnectionManager();
	virtual MPanLinkControlBase& BnepConnectionController();
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	virtual void SetRetryConnect(CPanRemoteDeviceStateMachine& aDevice);
	virtual void GetExistingConnections(TPanConnectionList& aPanConnectionList);
	virtual void CloseExistingConnectionWithUplinkAccess();
	virtual TBool WorthTrying(const TPanDeviceWorthTryingRolesList& aRemoteWorthTryingRoles) const;
	
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// MPanLinkControl methods
	virtual TAny* MplcbGetInterface(const TUid & aInterface);
	virtual TInt MplcRequestSniff(const TBTDevAddr &);
	virtual TInt MplcRequestActive(const TBTDevAddr &);
	
	MPanLinkControl* PanLinkControlInterface();
#endif	

	TBool AllowIncoming() const;
	void SetAllowIncoming(TBool aAllowIncoming);

	TBool NapEnabled() const;
	void SetNapEnabled(TBool aNapEnabled);	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
protected:
    // From ESock::MCFNode, via ESock::ACFNodeIdBase
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage);
#endif

private:
	CPanAgent();
	void ConstructL();
	
	CPanRemoteDeviceStateMachine* FindRemoteDeviceByAddress(const TBTDevAddr& remoteDeviceAddress);
	void CheckDeviceIsNotAlreadyAttachedL(const TBTDevAddr& aDevAddr);
	void RequestDisconnect(TInt aError);

	void InitiateDisconnect();
		
	static TInt InitiateDisconnectCb(TAny* aThisPtr);

	void SendUplinkAccessRequiredProgressNotification();
	
	TInt TryToCreateNewPanConnection();
	
private:
	RPointerArray<CPanRemoteDeviceStateMachine> iRemoteDevices;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    Messages::TNodeId iBnepConnectionManager;
#else
	MBnepConnectionManager* iBnepConnectionManager; //< Central management object in packet driver; we don't own this, so don't delete it!
#endif

	CPanIncomingConnectionListener* iIncomingConnectionListener;
	TBool iAllowIncoming;
	TBool iNapEnabled;
	
	CPanRemoteDeviceSelector* iDeviceSelector;		//< Device selector to handle all details of creating outgoing connections (commdb or prompting)

	CPanHelperLocalSdpRegistrar* iPanLocalSdpRegistrar;	//< Instance of local SDP registrar

	CPanAgtStateBase* iState; 						//< pointer to the state we're currently in
	CPanAgtStateBase* iShutdownState;				//< Preallocated shutdown state so we can always close down, even if we're OOM
	
	TInt iError; 									//< Used to store an error code to persist it during the callback process
	TBool iNifmanConnected; 						//< Indicate if we've progressed past ConnectComplete(), and therefore need to use a different error path to signal errors to nifman
	TBool iSuppressOutgoing;						//< Suppress any outgoing connections that may be started by this IAP - used to allow UIs to start the ad-hoc IAP at boot time without a prompt appearing
	
	TBluetoothPanRole iFixedLocalRole; 				//< If there is a local role set in commdb, then it is stored here (there will always be one set, but it might just be EPanRoleUnknown, in which case it is treated as unset)
	TBluetoothPanRole iFixedRemoteRole; 			//< If there is a remote role set in commdb, then it is stored here (there will always be one set, but it might just be EPanRoleUnknown, in which case it is treated as unset)

	CAsyncCallBack* iInitiateDisconnectCallback;
	
	TBool iUplinkRequiredProgressSent;				//< This is set when a uplink required progress is sent to the RConnection client.  This will be cleared by sending a uplink not required progress notification
	
	CPanCoexistenceConnectionController* iPanCoexistenceConnectionController; //< Manage co-existence with other IP bearers
	};
}	// PanAgent

#endif // PANAGT_H

