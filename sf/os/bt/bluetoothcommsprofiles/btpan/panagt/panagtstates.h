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

#ifndef PANAGTSTATES_H
#define PANAGTSTATES_H

/**
@file
@note PAN agent role state machine state definitions
*/

#include "panagt.h"
#include "panroles.h"

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

NONSHARABLE_CLASS(CPanAgtStateUninitialised) : public CPanAgtStateBase
/**
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

private:
	CPanAgtStateUninitialised(MPanAgtStateMachineNotify& aStateMachine);
	};

NONSHARABLE_CLASS(CPanAgentStateInitialising) : public CPanAgtStateBase
/**
Bootstrap the connection - create the incoming and outgoing connection starter objects as required
@note Outgoing connections must be started here, as opposed to the idle state, to allow transitions
back to the idle state without triggering more outgoing connections
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

protected:
	void DoStartOutgoingL();
private:
	CPanAgentStateInitialising(MPanAgtStateMachineNotify& aStateMachine);
	};
	
NONSHARABLE_CLASS(CPanAgtStateIdle) : public CPanAgtStateBase
/**
Idle state - waiting for role request from someone (either local or remote device(s))
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);
	
private:
	CPanAgtStateIdle(MPanAgtStateMachineNotify& aStateMachine);
public:

private:

	};
	
NONSHARABLE_CLASS(CPanAgtStateNegotiatingURole) : public CPanAgtStateBase
/**
Attempting to switch to U role
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);
	
protected:
	CPanAgtStateNegotiatingURole(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber);

private:
	CPanAgtStateNegotiatingURole(MPanAgtStateMachineNotify& aStateMachine);
public:

private:
	};
	
NONSHARABLE_CLASS(CPanAgtStateURole) : public CPanAgtStateBase
/**
Outgoing U role
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);
	virtual void RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

	virtual void StateSendRoleNotification();

private:
	CPanAgtStateURole(MPanAgtStateMachineNotify& aStateMachine);
public:

private:

	};

NONSHARABLE_CLASS(CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest) : public CPanAgtStateBase
/**
We want to act in the GN/NAP role, so renegotiate with the active device (of which there can only
be one)
@pre We were in U mode, and we decided we needed to go to GN/NAP (if the other end decides to request 
a role change, we handle it differently)
@note This occurs when a new device connects to us and we need to upgrade our role
@note Don't need to do any SDP U role deregistration in this state - there's an active device,
so we'll have already deregistered U role from SDP
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);
	virtual void RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole);
	virtual void PerformLocalRoleChangeRequestL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

private:
	CPanRemoteDeviceStateMachine& FindDeviceInActiveState();
	CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest(MPanAgtStateMachineNotify& aStateMachine);
public:

private:

	};
	
NONSHARABLE_CLASS(CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest) : public CPanAgtStateBase
/**
The remote device is asking us to move to U role
@note Don't need to do any SDP U role deregistration in this state - there's an active device,
so we'll have already deregistered U role from SDP
@note There might well be a transition from U role state -> this -> U role state. This can be 
caused by the peer trying to do a U-U -> U-GN/NAP upgrade (ie. we remain U throughout)
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

private:
	CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest(MPanAgtStateMachineNotify& aStateMachine);
public:

private:
	
	};

NONSHARABLE_CLASS(CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest) : public CPanAgtStateBase
/**
The remote device is asking us to move to GN/NAP role
@note Don't need to do any SDP U role deregistration in this state - there's an active device,
so we'll have already deregistered U role from SDP
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();

	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

private:
	CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest(MPanAgtStateMachineNotify& aStateMachine);
public:

private:
	
	};

	
NONSHARABLE_CLASS(CPanAgtStateNegotiatingGnOrNapRole) : public CPanAgtStateBase
/**
Attempting to switch to GN/NAP role
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();
	
	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

protected:
	CPanAgtStateNegotiatingGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber);
	
private:
	CPanAgtStateNegotiatingGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine);
public:

private:

	};

NONSHARABLE_CLASS(CPanAgtStateGnOrNapRole) : public CPanAgtStateBase
/**
GN/NAP role
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();
	
	// MRemoteDeviceNotify methods
	virtual void InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles);
	virtual void IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole);
	virtual void RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole);

	virtual void DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice);
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);

	virtual void StateSendRoleNotification();

private:
	CPanAgtStateGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine);	
public:

private:

	};
		
NONSHARABLE_CLASS(CPanAgtStateShutdown) : public CPanAgtStateBase
/**
Shutting down the whole PAN - disconnect everyone and give up
@note This class should only be created by the role state machine at startup. Shutdown should be performed
through the state machine using the preallocated class - this allows shutdown in OOM situations.
@note This class will always be entered from nifman calling Disconnect().  We can trigger this
from the role state machine (SM only, not the states) - states leave to trigger a disconnect.
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();
	
	// MRemoteDeviceNotify methods
	virtual void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);
	~CPanAgtStateShutdown();
private:
	void ConstructL();
	CPanAgtStateShutdown(MPanAgtStateMachineNotify& aStateMachine);

	static TInt ShutdownCallback(TAny* aPtr);

public:

private:
	CAsyncCallBack* iShutdownCallback;

	};

NONSHARABLE_CLASS(CPanAgtStateReconnect) : public CPanAgtStateBase
/**
Reconnect, a transient state
@internalComponent
*/
	{
public:
	static CPanAgtStateBase* NewL(MPanAgtStateMachineNotify& aStateMachine);
	void OnEntryL();
	
	void DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice);
	void Connect();
	void CancelReconnect();
	~CPanAgtStateReconnect();
private:
	void ConstructL();
	CPanAgtStateReconnect(MPanAgtStateMachineNotify& aStateMachine);
	static TInt ServiceStartedCb(TAny* aThisPtr);
	static TInt ReconnectCompleteCb(TAny* aThisPtr);
	void ServiceStarted();
private:
	CAsyncCallBack* iReconnectCompleteCallback;
	};


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
This state exists to pause the agent state machine whilst
we wait for an async response from BNEP
*/
NONSHARABLE_CLASS(CPanAgtStatePaused) : public CPanAgtStateBase
    {
public:
    static CPanAgtStatePaused* NewL(MPanAgtStateMachineNotify& aStateMachine, CPanAgtStateBase& aNextState);
    ~CPanAgtStatePaused();

	virtual void OnEntryL();
	void TransitionToNextState();
    
private:
    CPanAgtStatePaused(MPanAgtStateMachineNotify& aStateMachine, CPanAgtStateBase& aNextState);
    CPanAgtStateBase* iNextState; 
    };

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

} // namespace PanAgent

#endif
// PANAGTSTATES_H
