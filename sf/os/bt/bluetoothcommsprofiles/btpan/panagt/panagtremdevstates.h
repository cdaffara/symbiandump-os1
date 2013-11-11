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

#ifndef PANREMDEVSTATES_H
#define PANREMDEVSTATES_H

/**
@file
@note PAN agent remote connection state definitions
*/

#include "panagtremdev.h"
#include "panhelper.h"
#include "napuplinkauthorisationhelper.h"


namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

NONSHARABLE_CLASS(CPanRemDevStatePerformingSdpQuery) : public CPanRemDevStateBase
/**
Perform an SDP query to find out what roles the remote device supports
@note This is called for outgoing connections only
@internalComponent
*/
	{
public:
	CPanRemDevStatePerformingSdpQuery(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();
	
	// events caused by remote device
	// remote device is not connected in this state, so cannot send role request/responses, or disconnect

	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // for SDP query completion
	virtual void AsyncEventCancelL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:
	TPckg<TPanDeviceRolesList> iPanDevRolesListPtr; // same as above.  Needs to be in the class so it persists long enough for the server to use the package to read/write the variable
	};
	
NONSHARABLE_CLASS(CPanRemDevStateConnectingSocket) : public CPanRemDevStateBase
/**
Waiting for outgoing socket connect to complete
@note Only used when creating a new socket for an outgoing connection
@internalComponent
*/
	{
public:
	CPanRemDevStateConnectingSocket(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	// remote device is not connected in this state, so cannot send role request/responses, or disconnect
	
	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // for socket connect notification
	virtual void AsyncEventCancelL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:
	
	};

NONSHARABLE_CLASS(CPanRemDevStateReconnectingSocket) : public CPanRemDevStateBase
/**
Waiting for outgoing socket connect to complete
@note Only used when creating a new socket for an outgoing connection
@internalComponent
*/
	{
public:
	CPanRemDevStateReconnectingSocket(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	// remote device is not connected in this state, so cannot send role request/responses, or disconnect
	virtual void RemoteDeviceDisconnectL(TInt aError);
	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // for socket connect notification
	virtual void AsyncEventCancelL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:
	
	};
	
//
// Waiting-for-state-machine states
//

NONSHARABLE_CLASS(CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection) : public CPanRemDevStateBase
/**
Waiting for the local role state machine to be available to let us know what roles we can negotiate
@note This is for outgoing connections only, as you can tell from the name!
*/
	{
public:
	CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ReadyForRoleRequestL();
	virtual void ShutdownL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:
	
	};
		
NONSHARABLE_CLASS(CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection) : public CPanRemDevStateBase
/**
Waiting for the local role state machine to be available to let us know what roles we can negotiate
@note The remote device might send us role requests while we are in this state
@note This is for incoming connections only, as you can tell from the name!
*/
	{
public:
	CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ReadyForRoleRequestL();
	virtual void ShutdownL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

protected:
	CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);

private:

public:

private:
	
	};

// no CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingRoleChangeRequest because 
// if the state machine signals us that it wants to do a role change, it must not be locked (and we'll
// panic if it is)

NONSHARABLE_CLASS(CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest) : public CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection
/**
Waiting for the local role state machine to be available to let us know what roles we can negotiate
@note The remote device might send us role requests while we are in this state
@note This is for incoming role upgrades only, as you can tell from the name!
*/
	{
public:
	CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine, TBluetoothPanRole aPendingLocalRole, TBluetoothPanRole aPendingRemoteRole);
	virtual void OnEntryL();

	// events from local device
	virtual void ReadyForRoleRequestL();

private:

public:

private:
	TBluetoothPanRole iPendingLocalRole;
	TBluetoothPanRole iPendingRemoteRole;
	};

//
// Negotiating roles states
//

NONSHARABLE_CLASS(CPanRemDevStatePerformingRoleNegotiationForOutgoingBase) : public CPanRemDevStateBase
/**
Base class for outgoing role negotiation.
Performs resends of role request based on timer.
@internalComponent
*/
	{
public:
	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // for timer completion
	virtual void AsyncEventCancelL();
	
	// methods for derived classes to implement
	virtual CPanRemDevStateBase* DoRoleNegotiationSuccessL() = 0;
	virtual CPanRemDevStateBase* DoRoleNegotiationFailureL(TInt aError) = 0;

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

protected:
	CPanRemDevStatePerformingRoleNegotiationForOutgoingBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);

private:
	
protected:
	RTimer iTimer;	//< Timer to detect failure of remote device to respond to role request

private:
	TUint iRetries;	//< Counter to keep track of how many times we've retried the negotiation
	};

			
NONSHARABLE_CLASS(CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection) : public CPanRemDevStatePerformingRoleNegotiationForOutgoingBase
/**
Sends role request and waits for response from remote device
@internalComponent
*/
	{
public:
	CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// negotiation outcome methods
	virtual CPanRemDevStateBase* DoRoleNegotiationSuccessL();
	virtual CPanRemDevStateBase* DoRoleNegotiationFailureL(TInt aError);

protected:
	CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);
		
private:

public:
	
private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStatePerformingRoleNegotiationForIncomingConnection) : public CPanRemDevStateBase
/**
Wait for role request from other device, then send role response
@internalComponent
*/
	{
public:
	CPanRemDevStatePerformingRoleNegotiationForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ShutdownL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange) : public CPanRemDevStatePerformingRoleNegotiationForOutgoingBase
/**
Sends role request and waits for response from remote device
@internalComponent
*/
	{
public:
	CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);

	// negotiation outcome methods
	virtual CPanRemDevStateBase* DoRoleNegotiationSuccessL();
	virtual CPanRemDevStateBase* DoRoleNegotiationFailureL(TInt aError);

private:
	
public:

private:

	};

// no CPanRemDevStatePerformingRoleNegotiationForIncomingRoleChange class, because the role request arrives in
// the "active" state, and is handled there

NONSHARABLE_CLASS(CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection) : public CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection
/**
Performing role negotiation having been delayed because the local state machine was locked.
Sends role request and waits for response from remote device.
@internalComponent
*/
	{
public:
	CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();
	
	// don't need to handle response any differently from the "normal" case

private:
	
public:

private:

	};

//
// Role switching states
//

NONSHARABLE_CLASS(CPanRemDevStateWaitingForRoleSwitchBase) : public CPanRemDevStateBase, private MPanAgtTimerCallback
/**
Common base class for all role switching states.
This class contains the timer and role monitoring code, derived states implement the success and failure calls
that are pure virtual in this state.
@internalComponent
*/
	{
public:
	virtual ~CPanRemDevStateWaitingForRoleSwitchBase();
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole) = 0;
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode) = 0;
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // for timer and ioctl completion
	virtual void AsyncEventCancelL();

	// events from timer helper class
	virtual void TimerComplete();
	virtual void TimerError(TInt aError);

	// methods for derived classes to implement
	virtual CPanRemDevStateBase* DoRoleSwitchSuccessL() = 0;
	virtual CPanRemDevStateBase* DoRoleSwitchFailureL(TInt aError) = 0;

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

protected:
	CPanRemDevStateWaitingForRoleSwitchBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);

private:

public:

private:
	CPanAgtTimerHelper* iTimerHelper;		//< Timer helper class to detect failure of remote device to act upon a role switch	
	TBool iTimerCompleted;					//< Indication that the timer has fired
	TBTBasebandEvent iEventNotification; 	//< Buffer for notification of link events
	};
	
NONSHARABLE_CLASS(CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection) : public CPanRemDevStateWaitingForRoleSwitchBase
/**
We required a role switch on an outgoing connection and are now waiting for the baseband to let us know
@internalComponent
*/
	{
public:
	CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);

	// CPanRemDevStateWaitingForRoleSwitchBase methods
	virtual CPanRemDevStateBase* DoRoleSwitchSuccessL();
	virtual CPanRemDevStateBase* DoRoleSwitchFailureL(TInt aError);	

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

protected:
	CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);
	
private:

public:

private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStateWaitingForRoleSwitchForIncomingConnection) : public CPanRemDevStateWaitingForRoleSwitchBase
/**
We required a role switch on an incoming connection and are now waiting for the baseband to let us know
@internalComponent
*/
	{
public:
	CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);

	// CPanRemDevStateWaitingForRoleSwitchBase methods
	virtual CPanRemDevStateBase* DoRoleSwitchSuccessL();
	virtual CPanRemDevStateBase* DoRoleSwitchFailureL(TInt aError);	

protected:
	CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber);
	
private:

public:

private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest) : public CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection
/**
We required a role switch whilst doing an outgoing role change and are now waiting for the baseband to let us know
@internalComponent
*/
	{
public:
	CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();
	
	// events caused by remote device
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);

	// CPanRemDevStateWaitingForRoleSwitchBase methods
	virtual CPanRemDevStateBase* DoRoleSwitchSuccessL();
	virtual CPanRemDevStateBase* DoRoleSwitchFailureL(TInt aError);	

private:

public:

private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest) : public CPanRemDevStateWaitingForRoleSwitchForIncomingConnection
/**
We required a role switch whilst handling an incoming role change request and are now waiting for the baseband to let 
us know
@internalComponent
*/
	{
public:
	CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine, TBluetoothPanRole aPendingLocalRole, TBluetoothPanRole aPendingRemoteRole);
	virtual void OnEntryL();

	// CPanRemDevStateWaitingForRoleSwitchBase methods
	virtual CPanRemDevStateBase* DoRoleSwitchFailureL(TInt aError);	
	virtual CPanRemDevStateBase* DoRoleSwitchSuccessL();
	
private:

public:

private:
	TBluetoothPanRole iPendingLocalRole;
	TBluetoothPanRole iPendingRemoteRole;
	};

NONSHARABLE_CLASS(CPanRemDevStateActive) : public CPanRemDevStateBase
/**
The device is active - ie. BNEP negotiation has completed successfully
@internalComponent
*/	
	{
public:
	CPanRemDevStateActive(MPanRemDevStateMachineNotify& aStateMachine);
	virtual void OnEntryL();

	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);
	
	// events from local device
	virtual void ReadyForRoleRequestL();
	virtual void ShutdownL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:

public:

private:

	};
	
NONSHARABLE_CLASS(CPanRemDevStateShutdown) : public CPanRemDevStateBase
/**
Disconnect from the remote device and perform local cleanup actions
@note This state should always be entered from the state machine, as it is preallocated there to 
ensure we can always transition to it, even if we're OOM
@internalComponent
*/
	{
	friend class CPanRemoteDeviceStateMachine; // only the state machine can new up one of these
public:
	virtual void OnEntryL();
	
	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);

	// events from local device
	virtual void ShutdownL();
	virtual void AsyncEventCompleteL(); // to allow async shutdown
	virtual void AsyncEventCancelL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

private:
	CPanRemDevStateShutdown(MPanRemDevStateMachineNotify& aStateMachine);

public:

private:
	
	};


NONSHARABLE_CLASS(CPanRemDevStateIncomingNotification) : public CPanRemDevStateBase,
														 public MNapUplinkAuthorisationRequester
/**
@internalComponent
*/
	{
	friend class CPanRemoteDeviceStateMachine; // only the state machine can new up one of these
public:
	CPanRemDevStateIncomingNotification(MPanRemDevStateMachineNotify& aStateMachine);

	virtual void OnEntryL();
	
	// events caused by remote device
	virtual void BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole);
	virtual void BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode);
	virtual void RemoteDeviceDisconnectL(TInt aError);

	// events from local device
	virtual void ShutdownL();

	// This returns a distilled (i.e., not the actual remote device state)
	// version of the current connection status of the remote device
	virtual TRemoteDeviceState GetState() const;

	// From MNapUplinkAuthorisationRequester
	virtual void MnuarNapUplinkAuthorisationHelperComplete(TInt aError, TNapConnectionResult aResult);

	~CPanRemDevStateIncomingNotification();
	
private:
	// Helper method to safely manage the closing of the connection to the peer device.
	void ShutdownConnection(TInt aError);
	
public:

private:
	CNapUplinkAuthorisationHelper* iNapUplinkAuthorisationHelper;	
	
	// This flag is set when shutdown has been initiated.
	TBool iShutdownCalled;
	};


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
This state exists to pause the remote device state machine whilst
we wait for an async response from BNEP
*/
NONSHARABLE_CLASS(CPanRemDevStatePaused) : public CPanRemDevStateBase
    {
public:
    CPanRemDevStatePaused(MPanRemDevStateMachineNotify& aStateMachine, CPanRemDevStateBase& aNextState);
    ~CPanRemDevStatePaused();

    virtual void OnEntryL();
    virtual void ShutdownL();
    
    virtual void AsyncEventCompleteL();
    virtual void AsyncEventCancelL();
    
	virtual TRemoteDeviceState GetState() const;
    
private:
    CPanRemDevStateBase* iNextState; 
    };

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

} // namespace PanAgent


#endif
// PANREMDEVSTATES_H
