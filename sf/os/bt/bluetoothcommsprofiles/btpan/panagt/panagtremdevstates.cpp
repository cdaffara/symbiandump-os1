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
 @note PAN agent remote connection state implementations
*/

#include <bluetooth/logger.h>
#include <networking/panuiinterfaces.h>
#include "panagtremdevstates.h"
#include "panagtpolicy.h"
using namespace PanAgent;

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

//
// CPanRemDevStatePerformingSdpQuery
//

CPanRemDevStatePerformingSdpQuery::CPanRemDevStatePerformingSdpQuery(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStatePerformingSdpQuery), iPanDevRolesListPtr(StateMachine().RemoteRolesList())
/**

*/
	{
	}

void CPanRemDevStatePerformingSdpQuery::OnEntryL()
/**
Check out roles in commdb, perform SDP query if necessary, then check it's OK
@note If SDP queries are inhibited by the setting in commdb, this class will just accept whatever is in there - if there isn't anything then the SDP query will be performed anyway
*/
	{
	// perform SDP query to see what the remote device supports
	User::LeaveIfError(StateMachine().RemoteSdpQuerier().Query(StateMachine().RemSockAddr().BTAddr(), iPanDevRolesListPtr, StateMachine().Status()));
	StateMachine().SetActive();
	}

void CPanRemDevStatePerformingSdpQuery::AsyncEventCompleteL()
/**
The SDP query has completed
*/
	{
	//The PAN spec says a device supporting PANU does not have to advertise that on SDP.
	//But the spec is a bit vague on if such a device is connectable, so we try to connect
	//to it anyway to maximise interoperability
	if(StateMachine().Status()==KErrNone || StateMachine().Status()==KErrNotFound)
		{	
		// If remote doesn't support GN, there is no point try it, so set it to Not WorthTrying
		if(!StateMachine().RemoteRolesList().SupportsGn())
			{
			StateMachine().RemoteWorthTryingRolesList().SetWorthTryingGn(EFalse);
			}
		// If remote doesn't support NAP, there is no point try it, so set it to Not WorthTrying
		if(!StateMachine().RemoteRolesList().SupportsNap())
			{
			StateMachine().RemoteWorthTryingRolesList().SetWorthTryingNap(EFalse);
			}
			
		// check whether FixedRemoteRole is supported by remote according to SDP query result
		if(StateMachine().WorthTrying())
			{
			CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateConnectingSocket(StateMachine());
			LOG1(_L("RemDevState[%x]: <performing SDP query> - SDP query completed successfully, moving to <connecting socket> state"), &StateMachine());
			StateMachine().SetState(*nextState);
			delete this;
			}
		else
			{
			LOG1(_L("RemDevState[%x]: <performing SDP query> - SDP query completed successfully, but remote doesn't support the Fixed Remote Role we've specified."), &StateMachine());
			User::Leave(KErrFixRemoteRoleIsNotSupportedByRemote);
			}
		}
	else // error handling
		{
		LOG1(_L("RemDevState[%x]: <performing SDP query> - SDP query failed"), &StateMachine());
		User::Leave(StateMachine().Status().Int());
		}
	}

void CPanRemDevStatePerformingSdpQuery::AsyncEventCancelL()
/**
The SDP query has been cancelled
*/
	{
	LOG1(_L("RemDevState[%x]: <performing SDP query> - SDP query cancelled"), &StateMachine());

	StateMachine().RemoteSdpQuerier().CancelQuery();
	}
	
void CPanRemDevStatePerformingSdpQuery::ShutdownL()
/**
The local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <performing SDP query> - locally requested disconnect received"), &StateMachine());
	
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect);
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStatePerformingSdpQuery::GetState() const
	{
	return EIdle;
	}

//
// CPanRemDevStateConnectingSocket
//

CPanRemDevStateConnectingSocket::CPanRemDevStateConnectingSocket(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateConnectingSocket)
/**

*/
	{	}
		
void CPanRemDevStateConnectingSocket::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <connecting socket> - entering state"), &StateMachine());

	TBTServiceSecurity securitySettings;
	// we override whatever security settings that came with the address with our own
	securitySettings.SetAuthentication(KPanOutgoignAuthenticationRequired);
	securitySettings.SetAuthorisation(KPanOutgoingAuthorisationRequired);
	securitySettings.SetEncryption(KPanOutgoingEncryptionRequired);
	securitySettings.SetUid(KBTPanAuthorisationUid);

	StateMachine().RemSockAddr().SetSecurity(securitySettings);

#if defined(_DEBUG) && defined(SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY)
    TInt err = StateMachine().Socket().Open(KBTAddrFamily, KSockSeqPacket, KL2CAP);
    __ASSERT_DEBUG(err != KErrBadName, PanicInState(EPanAgtMissingBinding));
    User::LeaveIfError(err);
#else
	User::LeaveIfError(StateMachine().Socket().Open(KBTAddrFamily, KSockSeqPacket, KL2CAP));
#endif

	TUint16 bnepMtu = KBnepMtu;
	TPckg<TUint16> mtuBuf(bnepMtu);

	User::LeaveIfError(StateMachine().Socket().SetOpt(KL2CAPInboundMTU, KSolBtL2CAP, mtuBuf));
	User::LeaveIfError(StateMachine().Socket().SetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, mtuBuf));
	// Register the CoD service - Networking bit (especially relevant for WinXP)
	User::LeaveIfError(StateMachine().Socket().SetOpt(KBTRegisterCodService, KSolBtSAPBase, EMajorServiceNetworking));

	StateMachine().Socket().Connect(StateMachine().RemSockAddr(), StateMachine().Status());
	StateMachine().SetActive();
	}
	
void CPanRemDevStateConnectingSocket::AsyncEventCompleteL()
/**
Called when the new socket is connected
*/
	{
	if(StateMachine().Status()==KErrNone)
		{
		LOG1(_L("RemDevState[%x]: <connecting socket> - async event complete, socket connected successfully"), &StateMachine());

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        StateMachine().CreateNewBnepConnection(StateMachine().Socket(), TPanMessage::EActivityCreateChannelControllerForOutgoing);
#else
		StateMachine().CreateNewBnepConnectionL(StateMachine().Socket());
#endif

		StateMachine().OpenPhysicalLinkAdapterL();

		CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(StateMachine());

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        CleanupStack::PushL(nextState);
        CPanRemDevStatePaused* pausedState = new(ELeave)CPanRemDevStatePaused(StateMachine(), *nextState);
        CleanupStack::Pop();
        nextState = pausedState;
#endif

		StateMachine().SetState(*nextState);
		delete this;
		}
	else	// did not complete the connection to the remote device...
		{
		LOG2(_L("RemDevState[%x]: <connecting socket> - socket connect failed with %d"), &StateMachine(), StateMachine().Status().Int());
		User::Leave(StateMachine().Status().Int());
		}
	}	
	
void CPanRemDevStateConnectingSocket::AsyncEventCancelL()
/**
Cancel the socket connect
*/
	{
	LOG1(_L("RemDevState[%x]: <connecting socket> - async event cancelled"), &StateMachine());
	
	StateMachine().Socket().CancelConnect();
	StateMachine().Socket().Close();
	}

void CPanRemDevStateConnectingSocket::ShutdownL()
/**
The local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <connecting socket> - locally requested disconnect received"), &StateMachine());
	
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect);
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateConnectingSocket::GetState() const
	{
	return EIdle;
	}

//
// CPanRemDevStateReconnectingSocket
//

CPanRemDevStateReconnectingSocket::CPanRemDevStateReconnectingSocket(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateConnectingSocket)
	{	}
		
/**
	Disconnect the socket for the case where the remote device doesn't
*/
void CPanRemDevStateReconnectingSocket::OnEntryL()
	{
	LOG1(_L("RemDevState[%x]: <reconnecting socket> - entering state"), &StateMachine());
	StateMachine().Socket().Close();

	StateMachine().SetRetryConnect();	
	}

/**
	Wait for device to disconnect before attempting reconnect
*/
void CPanRemDevStateReconnectingSocket::RemoteDeviceDisconnectL(TInt /*aErr*/)
	{
	LOG1(_L("RemDevState[%x]: <reconnecting socket> - remote device disconnected"), &StateMachine());
	StateMachine().DisconnectBnepChannel();
	TBTServiceSecurity securitySettings;
	// we override whatever security settings that came with the address with our own
	securitySettings.SetAuthentication(KPanOutgoignAuthenticationRequired);
	securitySettings.SetAuthorisation(KPanOutgoingAuthorisationRequired);
	securitySettings.SetEncryption(KPanOutgoingEncryptionRequired);
	securitySettings.SetUid(KBTPanAuthorisationUid);

	StateMachine().RemSockAddr().SetSecurity(securitySettings);

#if defined(_DEBUG) && defined(SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY)
    TInt err = StateMachine().Socket().Open(KBTAddrFamily, KSockSeqPacket, KL2CAP);
    __ASSERT_DEBUG(err != KErrBadName, PanicInState(EPanAgtMissingBinding));
    User::LeaveIfError(err);
#else
	User::LeaveIfError(StateMachine().Socket().Open(KBTAddrFamily, KSockSeqPacket, KL2CAP));
#endif

	TUint16 bnepMtu = KBnepMtu;
	TPckg<TUint16> mtuBuf(bnepMtu);

	User::LeaveIfError(StateMachine().Socket().SetOpt(KL2CAPInboundMTU, KSolBtL2CAP, mtuBuf));
	User::LeaveIfError(StateMachine().Socket().SetOpt(KL2CAPNegotiatedOutboundMTU, KSolBtL2CAP, mtuBuf));

	StateMachine().Socket().Connect(StateMachine().RemSockAddr(), StateMachine().Status());
	StateMachine().SetActive();
	}

/**
Called when the new socket is connected
*/
void CPanRemDevStateReconnectingSocket::AsyncEventCompleteL()
	{
	if(StateMachine().Status()==KErrNone)
		{
		LOG1(_L("RemDevState[%x]: <connecting socket> - async event complete, socket connected successfully"), &StateMachine());

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        StateMachine().CreateNewBnepConnection(StateMachine().Socket(), TPanMessage::EActivityCreateChannelControllerForOutgoing);
#else
		StateMachine().CreateNewBnepConnectionL(StateMachine().Socket());
#endif

		StateMachine().OpenPhysicalLinkAdapterL();

		CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(StateMachine());

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        CleanupStack::PushL(nextState);
        CPanRemDevStatePaused* pausedState = new(ELeave)CPanRemDevStatePaused(StateMachine(), *nextState);
        CleanupStack::Pop();
        nextState = pausedState;
#endif

		StateMachine().SetState(*nextState);
		delete this;
		}
	else	// did not complete the connection to the remote device...
		{
		LOG2(_L("RemDevState[%x]: <connecting socket> - socket connect failed with %d"), &StateMachine(), StateMachine().Status().Int());
		User::Leave(StateMachine().Status().Int());
		}
	}	

/**
Cancel the socket connect
*/
void CPanRemDevStateReconnectingSocket::AsyncEventCancelL()
	{
	LOG1(_L("RemDevState[%x]: <connecting socket> - async event cancelled"), &StateMachine());
	
	StateMachine().Socket().CancelConnect();
	StateMachine().Socket().Close();
	}

/**
The local device wants us to disconnect
*/
void CPanRemDevStateReconnectingSocket::ShutdownL()
	{
	LOG1(_L("RemDevState[%x]: <connecting socket> - locally requested disconnect received"), &StateMachine());
	
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect);
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateReconnectingSocket::GetState() const
	{
	return EIdle;
	}

//
// CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection
//

CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection)
/**

*/
	{	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::OnEntryL()
/**
Not much to do here - just waiting for role state machine to become available
*/
	{	
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - entering state"), &StateMachine());

	// do nothing
	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Oi! We're starting the connection here, so we get to send the role request!
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - BNEP role request received from remote device - they shouldn't be sending them, we're doing the outgoing connection, disconnecting them"), &StateMachine());

	// drop the role request packet that the remote device has incorrectly sent us
	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Excuse me?  We haven't sent a role request yet, so why are they sending a role response?
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - BNEP role response received from remote device before we've sent a request - disconnecting them"), &StateMachine());

	// drop the role response packet that the remote device has incorrectly sent us
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::RemoteDeviceDisconnectL(TInt aError)
/**
Remote device has obviously given up waiting for us...
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - remote device disconnect received, looks like they've given up waiting for us."), &StateMachine());

	User::Leave(aError); // will shutdown the connection
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::ReadyForRoleRequestL()
/**
Ahh, that's the event we've really been waiting for - role state machine has finished dealing with
another device and is now ready for our role request
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - ready for role request received from role state machine"), &StateMachine());

	TInt err;
	CPanRemDevStateBase* nextState = NULL;
	StateMachine().RemoteRole() = EPanRoleUnknown;	// reset this for outgoing connection - we maybe trying a different role now
	err = StateMachine().InitiateOutgoingConnection(StateMachine().LocalRole(), StateMachine().RemoteRole(), StateMachine().RemoteWorthTryingRolesList());
		
	switch(err)
		{
		case KErrNone:
			{
			err = PerformMasterSlaveSwitchIfNecessary(StateMachine().LocalRole());
			if(err==KErrWaitingForBasebandRoleSwitch)
				{
				nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(StateMachine());
				break;
				}
			else if(err)
				{
				LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
				// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
				User::Leave(KErrCouldNotBecomePiconetMaster);
				return;		
				}
				
			LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - ...M/S switch not required, sending role request."), &StateMachine());

			nextState = new(ELeave) CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection(StateMachine());
			break;
			}
		case KErrLocked:
			{
			PanicInState(ERoleStateMachineCalledReadyForRoleRequestThenReturnedLocked);
			break;
			}
		case KErrInvalidOrUnacceptableRoleCombination:
			{
			LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - the roles the remote device proposed were invalid or unacceptable to us, disconnecting them"), &StateMachine());
			User::Leave(KErrInvalidOrUnacceptableRoleCombination);
			}
		default:
			{
			// if err is something other than KErrNoMemory that might indicate an internal error in the state
			// machine, so ASSERT_DEBUG.  We can probably carry on even if the error is unexpected, as leaving
			// will cause this connection to shutdown.
			__ASSERT_DEBUG(err == KErrNoMemory, PanicInState(EPanAgentRoleStateMachineReturnedUnexpectedErrorCode));
			User::Leave(err);
			}
		}
		
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::ShutdownL()
/**
Local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for outgoing connection> - locally requested disconnect received"), &StateMachine());

	User::Leave(KErrLocallyInitiatedDisconnect); // disconnect the remote device
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection::GetState() const
	{
	return EPendingNegotiation;
	}
	
//
// CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection
//

CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection)
/**

*/
	{	}

CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStateBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::OnEntryL()
/**
Not much to do here - just waiting for role state machine to become available
*/
	{	
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - entering state"), &StateMachine());

	// do nothing
	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - BNEP role request received from remote device, still can't do anything with it until we get access to the role state machine"), &StateMachine());

	// drop resent role request
	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
@note Shouldn't be called for incoming connections, as they should be sending requests, not responses
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - BNEP role response received from remote device, what are they up to?  They should be sending requests and we should be sending responses, disconnecting them"), &StateMachine());

	// drop the role response packet that the remote device has incorrectly sent us
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::RemoteDeviceDisconnectL(TInt aError)
/**
Remote device has obviously given up waiting for us...
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - remote device disconnect received, looks like they've given up waiting for us."), &StateMachine());

	User::Leave(aError); // will shutdown the connection
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::ReadyForRoleRequestL()
/**
Ahh, that's the event we've really been waiting for - role state machine has finished dealing with
another device and is now ready for our role request
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - ready for role request received from role state machine"), &StateMachine());

	TInt err;
	CPanRemDevStateBase* nextState = NULL;
	err = StateMachine().IncomingConnectionFromPeer(StateMachine().LocalRole(), StateMachine().RemoteRole());

	switch(err)
		{
		case KErrNone:
			{
			err = PerformMasterSlaveSwitchIfNecessary(StateMachine().LocalRole());
			if(err==KErrWaitingForBasebandRoleSwitch)
				{
				nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(StateMachine());
				break;
				}
			else if(err)
				{
				LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
				// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
				StateMachine().SendRoleResponse(EConnectionNotAllowed);
				User::Leave(KErrCouldNotBecomePiconetMaster);
				return;		
				}
				
			LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - ...M/S switch not required, sending successful connect response."), &StateMachine());
			StateMachine().SendRoleResponse(EOperationSuccessful);

 			nextState = new(ELeave) CPanRemDevStateIncomingNotification(StateMachine());
			break;
			}
		case KErrLocked:
			{
			PanicInState(ERoleStateMachineCalledReadyForRoleRequestThenReturnedLocked);
			break;
			}

		default:
			{
			// An error has occured.  Send a -ve role response and disconnect.
			LOG2(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - Fail with error [%d].  Disconnecting peer"), &StateMachine(), err);
			StateMachine().SendRoleResponse(EConnectionNotAllowed);
			User::Leave(err);
			}
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::ShutdownL()
/**
Local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming connection> - locally requested disconnect received"), &StateMachine());

	// tell the other end that we're disconnecting
	StateMachine().SendRoleResponse(EConnectionNotAllowed);
	// and then disconnect the remote device
	User::Leave(KErrLocallyInitiatedDisconnect); 
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection::GetState() const
	{
	return EPendingNegotiation;
	}

//
// CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest
//

CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest::CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine, TBluetoothPanRole aPendingLocalRole, TBluetoothPanRole aPendingRemoteRole)
  : CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(aStateMachine, EPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest),
	iPendingLocalRole(aPendingLocalRole),
	iPendingRemoteRole(aPendingRemoteRole)
/**

*/
	{	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest::OnEntryL()
/**
Not much to do here - just waiting for role state machine to become available
*/
	{	
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming role change request> - entering state"), &StateMachine());

	// do nothing
	}
	
void CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest::ReadyForRoleRequestL()
/**
Ahh, that's the event we've really been waiting for - role state machine has finished dealing with
another device and is now ready for our role request
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming role change request> - ready for role request received from role state machine"), &StateMachine());

	TInt err;
	CPanRemDevStateBase* nextState = NULL;
	err = StateMachine().RoleChangeRequestFromPeer(iPendingLocalRole, iPendingRemoteRole);
	switch(err)
		{
		case KErrNone:
			{
			err = PerformMasterSlaveSwitchIfNecessary(iPendingLocalRole);
			if(err==KErrWaitingForBasebandRoleSwitch)
				{
				nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest(StateMachine(), iPendingLocalRole, iPendingRemoteRole);
				break;
				}
			else if(err)
				{
				LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming role change request> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
				// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
				StateMachine().SendRoleResponse(EConnectionNotAllowed);
				StateMachine().RoleChangeFailed();
				nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
				break;
				}
				
			LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming role change request> - ...M/S switch not required, sending successful connect response."), &StateMachine());
			StateMachine().SendRoleResponse(EOperationSuccessful);

			// Store the new roles
			StateMachine().LocalRole() = iPendingLocalRole;
			StateMachine().RemoteRole() = iPendingRemoteRole;

			// Changing role from GN/NAP to U.  If this connection previously had access to the
			// uplink this should be revoked.
			if(StateMachine().LocalRole() == EPanRoleU)
				{
				StateMachine().SetUplinkAccessAllowed(EFalse);
				}

			nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
			break;
			}
		case KErrLocked:
			{
			PanicInState(ERoleStateMachineCalledReadyForRoleRequestThenReturnedLocked);
			break;
			}
		case KErrInvalidOrUnacceptableRoleCombination:
			{
			LOG1(_L("RemDevState[%x]: <waiting for role state machine for incoming role change request> - the roles the remote device proposed were invalid or unacceptable to us, signalling failure and going to active state"), &StateMachine());
			StateMachine().SendRoleResponse(EConnectionNotAllowed);
			nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
			break;
			}
		default:
			{
			// if err is something other than KErrNoMemory that might indicate an internal error in the state
			// machine, so ASSERT_DEBUG.  We can probably carry on even if the error is unexpected, as leaving
			// will cause this connection to shutdown.
			__ASSERT_DEBUG(err == KErrNoMemory, PanicInState(EPanAgentRoleStateMachineReturnedUnexpectedErrorCode));
			User::Leave(err);
			}
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;	
	}

//
// CPanRemDevStatePerformingRoleNegotiationForOutgoingBase
//

CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::CPanRemDevStatePerformingRoleNegotiationForOutgoingBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStateBase(aStateMachine, aStateNumber), iRetries(KMaxBnepNegotiationCyles)
/**

*/
	{	}

void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Something's gone wrong - we were expecting a response from the remote device - not a request. 
Not sure this can/should occur - I think the spec implies that only the initiating device can send role requests.
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - BNEP role request from remote device received - what are they doing?  We're supposed to be sending the role requests."), &StateMachine());

	// drop the role request packet that the remote device has incorrectly sent us
	}
	
void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode)
/**
Woohoo! Our role response has arrived.
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - BNEP role response from remote device received, processing response..."), &StateMachine());

	StateMachine().Cancel();
	
	CPanRemDevStateBase* nextState = NULL;
	switch(aRoleResponseCode)
		{
		case EOperationSuccessful:
			{
			LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation successful, setting device active."), &StateMachine());
			StateMachine().ResetConnectionRetryAttempts(); // reset the connection retry attempts
			StateMachine().ResetRetryParameters(); // clears the triedpanu... type flags incase we want to reconnect later
			nextState = DoRoleNegotiationSuccessL();
			break;
			}
		case EConnectionNotAllowed:
			{	
			LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, connection not allowed"), &StateMachine());
			TInt retries = StateMachine().IncrementConnectionRetryAttempts();
			// There is a race condition in some PC stacks that can bring us to this point when it shouldn't, so we retry the connection
			if (retries<KPanAgtConnectionRetryAttempts) 
				{
				LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, connection not allowed - retrying connection"), &StateMachine());
				StateMachine().ResetRetryParameters(); // This feeds all the way back so we recalculate the role parameters just the same as we did last time
				nextState = new(ELeave) CPanRemDevStateReconnectingSocket(StateMachine());
			
				// if the state has left, then it won't have performed the setup of the next state correctly
				// this code performs the same actions to cleanup the old state and activate the new, but in
				// reverse order - otherwise we'd lose the pointer to the old state when we activated the new
				}
			/* the intention is to allow the retrys required by the PC stacks above
			   then if it's worth retrying (we can try new roles) we will
			   This is particularly useful when we don't have sdp records to suggest
			   what we should do
			 */				
			else if (StateMachine().WorthTrying())
				{
				// if we've still not made a successful connection then we can have a quick try
				// at connecting using different remote roles
				LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, connection not allowed - retrying connection with different roles"), &StateMachine());
				nextState = new(ELeave) CPanRemDevStateReconnectingSocket(StateMachine());
			
				// if the state has left, then it won't have performed the setup of the next state correctly
				// this code performs the same actions to cleanup the old state and activate the new, but in
				// reverse order - otherwise we'd lose the pointer to the old state when we activated the new
				}
			else 
				{
				LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, connection not allowed"), &StateMachine());
				LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, unrecognised response code, shutting down"), &StateMachine());
				StateMachine().ResetConnectionRetryAttempts(); // reset the connection retry attempts
				StateMachine().ResetRetryParameters(); // clears the triedpanu... type flags incase we want to try connect later
				nextState = DoRoleNegotiationFailureL(KErrCouldNotConnect);
				}
			break;
			}
		default:
			{			
			// this nested switch avoids the old multiple logging lines which was really nasty
			switch(aRoleResponseCode)
				{
				case EInvalidDestinationServiceUuid:	// we've tried to negotiate roles that are unsuitable
					{
					LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, invalid destination service UUID"), &StateMachine());
					break;
					}
				case EInvalidSourceServiceUuid:
					{
					LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, invalid source service UUID"), &StateMachine());
					break;
					}
				case EInvalidServiceUuidSize:
					{
					LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, invalid UUID size"), &StateMachine());
					break;
					}
				default:
					{
					LOG2(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...operation failed, unrecognised response code (%x),  shutting down"), &StateMachine(),aRoleResponseCode);
					break;
					}
				}
			StateMachine().ResetConnectionRetryAttempts(); // reset the connection retry attempts
			StateMachine().ResetRetryParameters(); // clears the triedpanu... type flags incase we want to try connect later
			nextState = DoRoleNegotiationFailureL(KErrCouldNotConnect);
			break;	
			}
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}
	
void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::RemoteDeviceDisconnectL(TInt aError)
/**
Remote device has disconnected.
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - remote device disconnect received, looks like our requests weren't acceptable, or they just gave up"), &StateMachine());

	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(aError); // will shutdown the connection
	}	

void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::ShutdownL()
/**
Local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - locally requested disconnect received"), &StateMachine());
	
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect); // will shutdown the connection
	}
	
void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::AsyncEventCompleteL()
/**
Timer completion
*/
	{
	--iRetries;

	LOG2(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - async event complete, looks like the timer triggered before they responded.  %d retries remaining..."), &StateMachine(), iRetries);

	if(iRetries==0)
		{
		LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...disconnecting."), &StateMachine());
		CPanRemDevStateBase* nextState = DoRoleNegotiationFailureL(KErrRemoteDeviceFailedToRespondToRoleRequests);
		StateMachine().SetState(*nextState);
		delete this;
		return;
		}
	else	// resend the role request, and start the timer running again
		{
		LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - ...retrying."), &StateMachine());
		StateMachine().SendRoleRequest(StateMachine().LocalRole(), StateMachine().RemoteRole());
		iTimer.After(StateMachine().Status(), KMaxTimeToWaitForRoleResponse);
		StateMachine().SetActive();
		}
	}
	
void CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::AsyncEventCancelL()
/**
Timer cancel
@note Typically it will be this state that cancels the async event (timer in this case) when we decide we don't need it any longer
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation [base]> - async event cancelled, cancelling timer"), &StateMachine());

	iTimer.Cancel();
	iTimer.Close();
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStatePerformingRoleNegotiationForOutgoingBase::GetState() const
	{
	return EPerformingNegotiation;
	}
	
//
// CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection
//

CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection::CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStatePerformingRoleNegotiationForOutgoingBase(aStateMachine, EPanRemDevStatePerformingRoleNegotiationForOutgoingConnection)
/**

*/
	{	}

CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection::CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStatePerformingRoleNegotiationForOutgoingBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}

void CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection::OnEntryL()
/**
Send the role request to the remote device
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - entering state"), &StateMachine());
	
	User::LeaveIfError(iTimer.CreateLocal());
		
	TInt err;	
	CPanRemDevStateBase* nextState = NULL;
	StateMachine().RemoteRole() = EPanRoleUnknown;	// reset this for outgoing connection - we maybe trying a different role now
	err = StateMachine().InitiateOutgoingConnection(StateMachine().LocalRole(), StateMachine().RemoteRole(), StateMachine().RemoteWorthTryingRolesList());

	switch(err)
		{
		case KErrNone:
			{
			LOG3(_L("RemDevState[%x]: <performing outgoing role negotiation> - role state machine changed our proposed role; now local: %x, remote: %x"), &StateMachine(), StateMachine().LocalRole(), StateMachine().RemoteRole());
			err = PerformMasterSlaveSwitchIfNecessary(StateMachine().LocalRole());
			
			if(err==KErrWaitingForBasebandRoleSwitch)
				{
				nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(StateMachine());
				break;
				}
			else if(err)
				{
				LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
				// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
				User::Leave(KErrCouldNotBecomePiconetMaster);
				return;		
				}
				
			StateMachine().SendRoleRequest(StateMachine().LocalRole(), StateMachine().RemoteRole());
			iTimer.After(StateMachine().Status(), KMaxTimeToWaitForRoleResponse);
			StateMachine().SetActive();
			return;
			}
		case KErrLocked:	// someone else is negotiating, so go to the wait state
			{
			LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - role state machine busy, waiting for notification of its availability"), &StateMachine());
			nextState = new(ELeave) CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForOutgoingConnection(StateMachine());
			break;
			}

		case KErrInvalidOrUnacceptableRoleCombination:
			LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - this state generated and proposed an illegal role combination"), &StateMachine());
			// fall through
			
		default:
			LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - unexpected response from role state machine"), &StateMachine());
			// if err is something other than KErrNoMemory that might indicate an internal error in the state
			// machine, so ASSERT_DEBUG.  We can probably carry on even if the error is unexpected, as leaving
			// will cause this connection to shutdown.
			User::Leave(err);
		}
		
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

CPanRemDevStateBase* CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection::DoRoleNegotiationSuccessL()
/**
Role negotiation succeeded, tell role state machine and go to active state
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - role negotiation succeeded, going to active state"), &StateMachine());

	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
	return nextState;
	}
	
CPanRemDevStateBase* CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection::DoRoleNegotiationFailureL(TInt aError)
/**
Role negotiation failed, disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <performing outgoing role negotiation> - role negotiation failed, shutting down connection"), &StateMachine());
	User::Leave(aError);
	return NULL;	// will never be reached	
	}
		
//
// CPanRemDevStatePerformingRoleNegotiationForIncomingConnection
//

CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::CPanRemDevStatePerformingRoleNegotiationForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStatePerformingRoleNegotiationForIncomingConnection)
/**

*/
	{	}

void CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - entering state"), &StateMachine());

	// do nothing - just wait for incoming role request
	}

void CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole)
/**
This is what we're waiting for...
*/
	{
//	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - BNEP role request from remote device, they propose local: %S, remote: %S"), &StateMachine(), (aRequestedLocalRole.ShortestForm()), (aRequestedRemoteRole.ShortestForm()));

	TInt err;
	CPanRemDevStateBase* nextState = NULL;
	
	TBluetoothPanRole localRole;
	TBluetoothPanRole remoteRole;
	ConvertUuidsToPanRolesL(aRequestedLocalRole, aRequestedRemoteRole, localRole, remoteRole); // will send response to remote device and shutdown the connection if there is an error

	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - checking role request..."), &StateMachine());

	err = StateMachine().IncomingConnectionFromPeer(localRole, remoteRole);

	// store the requested roles, so the next state knows what was asked for
	StateMachine().LocalRole() = localRole;
	StateMachine().RemoteRole() = remoteRole;

	switch(err)
		{
		case KErrNone:
			{
			LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - ...successful, checking to see if M/S switch necessary..."), &StateMachine());

			err = PerformMasterSlaveSwitchIfNecessary(localRole);
			if(err==KErrWaitingForBasebandRoleSwitch)
				{
				nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(StateMachine());
				break;
				}
			else if(err)
				{
				LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
				// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
				StateMachine().SendRoleResponse(EConnectionNotAllowed);
				User::Leave(KErrCouldNotBecomePiconetMaster);
				return;		
				}
				
			LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - ...M/S switch not required, sending successful connect response."), &StateMachine());
			StateMachine().SendRoleResponse(EOperationSuccessful);

			// Check if the incoming notifier is required.
 			nextState = new(ELeave) CPanRemDevStateIncomingNotification(StateMachine());
			break;
			}
		case KErrLocked:
			{
			LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - ...role state machine busy, waiting."), &StateMachine());
			// and create the "waiting" state
			nextState = new (ELeave) CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingConnection(StateMachine());
			break;
			}

		default:
			{
			// An error has occured.  Send a -ve role response and disconnect.
			LOG2(_L("RemDevState[%x]: <CPanRemDevStatePerformingRoleNegotiationForIncomingConnection> - Fail with error [%d].  Disconnecting peer"), &StateMachine(), err);
			StateMachine().SendRoleResponse(EConnectionNotAllowed);
			User::Leave(err);
			}
		}
	
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	}
	
void CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Something's gone wrong - we were expecting a request from the remote device - not a response.
*/
	{
	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - BNEP role reponse from remote device - what are they up to?  They're supposed to be sending the role request."), &StateMachine());
	
	// drop the role response packet that the remote device has incorrectly sent us
	}
	
void CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::RemoteDeviceDisconnectL(TInt aError)
/**
Remote device has disconnected
*/
	{
	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - remote device disconnect received, they've obviously given up talking to us."), &StateMachine());

	User::Leave(aError); // will shutdown the connection
	}
	
void CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::ShutdownL()
/**
Local device wants us to disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <performing incoming role negotiation> - locally requested disconnect received"), &StateMachine());

	User::Leave(KErrLocallyInitiatedDisconnect); // will shutdown the connection
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStatePerformingRoleNegotiationForIncomingConnection::GetState() const
	{
	return EPerformingNegotiation;
	}

//
// CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange
//

CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange::CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStatePerformingRoleNegotiationForOutgoingBase(aStateMachine, EPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange)
/**

*/
	{	}

void CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange::OnEntryL()
	{
	LOG1(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - entering state"), &StateMachine());

	User::LeaveIfError(iTimer.CreateLocal());

	// It may appear as though we ought to do a new SDP query, rather than relying on a potentially
	// stale one, but in reality if we're being asked to renegotiate, then the roles are pretty much 
	// going to be known by the role state machine, so we just ask it
	TInt err = StateMachine().PerformLocalRoleChangeRequest(StateMachine().LocalRole(), StateMachine().RemoteRole());
	switch(err)
		{
		case KErrNone:
			{
			LOG3(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - role state machine changed our proposed role; now local: %x, remote: %x"), &StateMachine(), StateMachine().LocalRole(), StateMachine().RemoteRole());

			// send the role request and wait for a response first, so the remote device 
			// understands that we're trying to role switch the baseband because we want to be GN/NAP,
			// and releases their piconet master lock
			StateMachine().SendRoleRequest(StateMachine().LocalRole(), StateMachine().RemoteRole());
							
			// start waiting for the role response
			iTimer.After(StateMachine().Status(), KMaxTimeToWaitForRoleResponse);
			StateMachine().SetActive();
			return;
			}
		case KErrLocked:
		// this must not happen - if the state machine requests that we renegotiate, then it needs to be ready for us
			{
			PanicInState(ERoleStateMachineRequestedWeRenegotiateRolesThenReturnedLocked);
			}
		case KErrInvalidOrUnacceptableRoleCombination:
			// Unable to find a valid role combination to send to the peer device.  fall through

		default:
			LOG1(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - unexpected response from role state machine"), &StateMachine());
			User::Leave(err);
			break;
		}
	}

void CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode)
/**
The role response has arrived from the peer device.
*/
	{
	LOG1(_L("RemDevState[%x]: <CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange> - BNEP role response from remote device received, processing response..."), &StateMachine());

	StateMachine().Cancel();
	
	CPanRemDevStateBase* nextState = NULL;
	switch(aRoleResponseCode)
		{
		case EOperationSuccessful:
			{
			LOG1(_L("RemDevState[%x]: <CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange> - ...operation successful, setting device active."), &StateMachine());
			nextState = DoRoleNegotiationSuccessL();
			break;
			}
		default:
			{	
			LOG2(_L("RemDevState[%x]: <CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange> - ...operation failed [result code = %d], connection not allowed"), &StateMachine(), aRoleResponseCode);
			nextState = DoRoleNegotiationFailureL(KErrCouldNotConnect);
			break;	
			}
		};
		
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

CPanRemDevStateBase* CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange::DoRoleNegotiationSuccessL()
/**
Role negotiation succeeded, decide whether we need to perform a baseband role switch
*/
	{
	LOG1(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - role negotiation succeeded, deciding whether we need to role switch"), &StateMachine());

	TInt err;
	// start going through the M/S switch routine
	err = PerformMasterSlaveSwitchIfNecessary(StateMachine().LocalRole());
	
	CPanRemDevStateBase* nextState = NULL;
	if(err==KErrWaitingForBasebandRoleSwitch)
		{
		nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest(StateMachine());
		}
	else if(err)
		{
		LOG1(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
		// remote device would not perform role switch (maybe it needs to stay as master for another 
		// profile it's using...)
		// Whilst the spec says we should return to previous roles when this role upgrade fails, 
		// we're now in the situation that we're GN/NAP, they're U, and we can't become the piconet 
		// master - so the best/most reliable/only way to recover is to disconnect them.  Not 
		// entirely according-to-spec, but then they shouldn't let us become master then refuse 
		// to role switch!
		User::Leave(KErrCouldNotBecomePiconetMaster);
		}
	else
		{
		nextState = new(ELeave) CPanRemDevStateActive(StateMachine());		
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	return nextState;
	}
	
CPanRemDevStateBase* CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange::DoRoleNegotiationFailureL(TInt /*aError*/)
/**
Role negotiation failed, return to active state
*/
	{
	LOG1(_L("RemDevState[%x]: <performing role negotiation for outgoing role change> - role negotiation failed, returning to active state"), &StateMachine());

	// let the role state machine know that the role change failed
	StateMachine().RoleChangeFailed();
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
	return nextState;
	}

//
// CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection
//

CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection::CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStatePerformingRoleNegotiationForOutgoingConnection(aStateMachine, EPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection)
/**

*/
	{	}
	
void CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection::OnEntryL()
/**
Send the role request and start the timer
*/
	{
	LOG1(_L("RemDevState[%x]: <performing delayed role negotiation for outgoing connection> - entering state"), &StateMachine());

	User::LeaveIfError(iTimer.CreateLocal());
	
	// send the role request to the remote device
	StateMachine().SendRoleRequest(StateMachine().LocalRole(), StateMachine().RemoteRole());
	iTimer.After(StateMachine().Status(), KMaxTimeToWaitForRoleResponse);
	StateMachine().SetActive();
	}
	
//
// CPanRemDevStateWaitingForRoleSwitchBase
//

CPanRemDevStateWaitingForRoleSwitchBase::CPanRemDevStateWaitingForRoleSwitchBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStateBase(aStateMachine, aStateNumber)
/**

*/
	{	}
	
CPanRemDevStateWaitingForRoleSwitchBase::~CPanRemDevStateWaitingForRoleSwitchBase()
/**
We own an object, so need our own destructor here to delete it
(this is the only state that does/needs this)
*/
	{
	delete iTimerHelper;
	iTimerHelper = NULL;
	}
	
void CPanRemDevStateWaitingForRoleSwitchBase::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch [base]> - entering state"), &StateMachine());

	//Proceed to the next state if role switch is not required
	TUint32 state;
	StateMachine().PhysicalLinkAdapter().PhysicalLinkState(state);
	if(state & ENotifyMaster)
		{
		CPanRemDevStateBase *nextState = DoRoleSwitchSuccessL();
		__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
		StateMachine().SetState(*nextState);
		delete this;
		return;
		}

	// create and start the timer helper object
	iTimerHelper = CPanAgtTimerHelper::NewL(*this);
	iTimerHelper->SetTimer(KMaxTimeToWaitForBasebandRoleSwitch);
	
	// subscribe to the link status change event notification
	iEventNotification() = ENotifyMaster;	// must set this to the event we're interested in, otherwise we don't see it
	StateMachine().PhysicalLinkAdapter().NotifyNextBasebandChangeEvent(iEventNotification, StateMachine().Status());
	StateMachine().SetActive();
	}

void CPanRemDevStateWaitingForRoleSwitchBase::RemoteDeviceDisconnectL(TInt aError)
/**
Oh well, they've given up
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch> - remote device disconnect received, they've obviously given up talking to us."), &StateMachine());
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(aError); // will shutdown the connection
	}

void CPanRemDevStateWaitingForRoleSwitchBase::ShutdownL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch> - locally requested disconnect received"), &StateMachine());
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect); // will shutdown the connection
	}

void CPanRemDevStateWaitingForRoleSwitchBase::AsyncEventCompleteL()
/**
Timer or baseband change event notification completion
*/
	{
	CPanRemDevStateBase* nextState = NULL;
	if(StateMachine().Status()==KErrNone)
		{
		if(iTimerCompleted)	// work out whether the timer fired or the baseband change event notification completed
			{
			// timer fired
			LOG1(_L("RemDevState[%x]: <waiting for role switch> - timer fired, giving up and disconnecting remote device"), &StateMachine());
			
			// cancel the outstanding request for baseband change event notification
			// need to cancel it directly, as the timer completes the state machine AO (through
			// the TimerComplete() and TimerCancel() methods, so just calling 
			// StateMachine().Cancel() won't have any effect
			StateMachine().PhysicalLinkAdapter().CancelNextBasebandChangeEventNotifier();

			User::WaitForRequest(StateMachine().Status());	// have to catch the KErrCancel manually, as we're not active at the moment

			nextState = DoRoleSwitchFailureL(KErrNone);
			}
		else
			{
			// Baseband change event notification completed
			if(iEventNotification().EventType() & ENotifyMaster)
				{
				LOG2(_L("RemDevState[%x]: <waiting for role switch> - received link event %32b, now piconet master - sending role response and going to active state"), &StateMachine(), iEventNotification().EventType());

				// cancel the timer
				iTimerHelper->Cancel(); // need to cancel it directly, as the state machine AO is
										// no longer active, so just cancelling all async events 
										// through the state machine won't work

				// success - get this state to do it's stuff
				// they *must* return the next state
				nextState = DoRoleSwitchSuccessL();
				}
			else // event that we're not interested in, repost 'baseband change event notification' request
				{
				iEventNotification() = ENotifyMaster; // must set this to the event we're interested in, otherwise we don't see it
				StateMachine().PhysicalLinkAdapter().NotifyNextBasebandChangeEvent(iEventNotification, StateMachine().Status());
				LOG2(_L("RemDevState[%x]: <waiting for role switch> - received link event %32b, throwing away"), &StateMachine(), iEventNotification().EventType());
				StateMachine().SetActive();
				return;
				}
			}
		}
	else
		{
		StateMachine().Cancel();
		nextState = DoRoleSwitchFailureL(StateMachine().Status().Int());
		}
		
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}
	
void CPanRemDevStateWaitingForRoleSwitchBase::AsyncEventCancelL()
/**
Cancel all outstanding async events
@note This only works when the state machine AO is active - ie. calling StateMachine().Cancel()
from other code is only any good when *both* timer and baseband change event notification are outstanding, because the
completion of either will mark the state machine AO as inactive
*/
	{
	iTimerHelper->Cancel();
	StateMachine().PhysicalLinkAdapter().CancelNextBasebandChangeEventNotifier();
	}
	
void CPanRemDevStateWaitingForRoleSwitchBase::TimerComplete()
/**
The timer has completed
*/
	{
	iTimerCompleted = ETrue;
	TRequestStatus* status = &StateMachine().Status();
	User::RequestComplete(status, KErrNone);
	}
	
void CPanRemDevStateWaitingForRoleSwitchBase::TimerError(TInt aError)
/**
There was a problem with the timer
*/
	{
	iTimerCompleted = ETrue;
	TRequestStatus* status = &StateMachine().Status();
	User::RequestComplete(status, aError);
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateWaitingForRoleSwitchBase::GetState() const
	{
	return EPerformingNegotiation;
	}
	
//
// CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection
//

CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateWaitingForRoleSwitchBase(aStateMachine, EPanRemDevStateWaitingForRoleSwitchForOutgoingConnection)
/**

*/
	{	}

CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStateWaitingForRoleSwitchBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}
		
void CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing connection> - entering state"), &StateMachine());
	
	// call base class's method
	CPanRemDevStateWaitingForRoleSwitchBase::OnEntryL();
	}

void CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Ignore it, we're waiting for them to role switch
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing connection> - BNEP role request from remote device: ignoring, as we're waiting for baseband role switch"), &StateMachine());

	// do nothing
	}
	
void CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Err, what's going on?  
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing connection> - BNEP role response from remote device: huh?  What are they up to?  This is an incoming connection, so they're supposed to be sending the requests, not the responses"), &StateMachine());

	// drop the role response packet that the remote device has incorrectly sent us
	}

CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::DoRoleSwitchSuccessL()
/**
It worked - now perform the role negotiation
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing connection> - role switch succeeded"), &StateMachine());
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingDelayedRoleNegotiationForOutgoingConnection(StateMachine());
	return(nextState);
	}
	
CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::DoRoleSwitchFailureL(TInt aError)	
/**
Shutdown the connection
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing connection> - role switch failed, shutting down"), &StateMachine());
	if(aError==KErrNone)	// the timer has fired
		{
		User::Leave(KErrCouldNotBecomePiconetMaster);
		}
	else					// a more general error occured
		{
		User::Leave(aError);
		}
	return NULL; // should never be reached
	}


// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection::GetState() const
	{
	return EPendingNegotiation;
	}

//
// CPanRemDevStateWaitingForRoleSwitchForIncomingConnection
//

CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateWaitingForRoleSwitchBase(aStateMachine, EPanRemDevStateWaitingForRoleSwitchForIncomingConnection)
/**

*/
	{	}

CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
	CPanRemDevStateWaitingForRoleSwitchBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}
		
void CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming connection> - entering state"), &StateMachine());
	
	// call base class's method
	CPanRemDevStateWaitingForRoleSwitchBase::OnEntryL();
	}

void CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Ignore it, we're waiting for them to role switch
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming [base]> - BNEP role request from remote device: ignoring, as we're waiting for baseband role switch"), &StateMachine());

	// ignore resent role request
	}
	
void CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Err, what's going on?  
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming [base]> - BNEP role response from remote device: huh?  What are they up to?  This is an incoming connection, so they're supposed to be sending the requests, not the responses"), &StateMachine());

	// drop the role response packet that the remote device has incorrectly sent us
	}

CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::DoRoleSwitchSuccessL()
/**
It worked - tell the remote device and go to active state
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming [base]> - role switch succeeded"), &StateMachine());

  	// tell the remote device the connection was successful
	StateMachine().SendRoleResponse(EOperationSuccessful);
	// let the role state machine know that this connection is now active
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateIncomingNotification(StateMachine());
	return(nextState);
	}
	
CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForIncomingConnection::DoRoleSwitchFailureL(TInt aError)	
/**
Shutdown the connection
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming connection> - role switch failed, shutting down"), &StateMachine());
	
	//send response DEF068158
 	StateMachine().SendRoleResponse(EConnectionNotAllowed); 	
 	//and cancel next base band change event notifier DEF067708 
	
	StateMachine().PhysicalLinkAdapter().CancelNextBasebandChangeEventNotifier();
	
	
	if(aError==KErrNone)	// the timer has fired
		{
		User::Leave(KErrCouldNotBecomePiconetMaster);
		}
	else					// a more general error occured
		{
		User::Leave(aError);
		}
	return NULL; // should never be reached
	}

//
// CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest
//

CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest::CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateWaitingForRoleSwitchForOutgoingConnection(aStateMachine, EPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest)
/**

*/
	{	}
		
void CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing role change request> - entering state"), &StateMachine());
	
	// call base class's method
	CPanRemDevStateWaitingForRoleSwitchBase::OnEntryL();
	}
	
void CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
They've responded to our role request twice!
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing role change request> - BNEP role response from remote device, they've already replied once - disconnecting them!"), &StateMachine());

	// drop the role response packet that the remote device has incorrectly sent us
	}
	
CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest::DoRoleSwitchSuccessL()
/**
We've managed to role switch, so the role upgrade has completed successfully
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing role change request> - role switch succeeded, going to active state"), &StateMachine());

	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
 	return nextState;
	}

CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForOutgoingRoleChangeRequest::DoRoleSwitchFailureL(TInt /*aError*/)	
/**
Role switch failed, but this was a role change so return to old role in active state
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for outgoing role change request> - role change failed, disconnecting remote device"), &StateMachine());

	// Whilst the spec says we should return to previous roles when this role upgrade fails, 
	// we're now in the situation that we're GN/NAP, they're U, and we can't become the piconet 
	// master - so the best/most reliable/only way to recover is to disconnect them.  Not 
	// entirely according-to-spec, but then they shouldn't let us become master then refuse 
	// to role switch!
	User::Leave(KErrCouldNotBecomePiconetMaster);
	return NULL;	// will never get here
	}

//
// CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest
//

CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest::CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest(MPanRemDevStateMachineNotify& aStateMachine, TBluetoothPanRole aPendingLocalRole, TBluetoothPanRole aPendingRemoteRole) 
  : CPanRemDevStateWaitingForRoleSwitchForIncomingConnection(aStateMachine, EPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest),
	iPendingLocalRole(aPendingLocalRole),
	iPendingRemoteRole(aPendingRemoteRole)
  
/**

*/
	{	}
		
void CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming role change request> - entering state"), &StateMachine());
	
	// call base class's method
	CPanRemDevStateWaitingForRoleSwitchBase::OnEntryL();
	}

CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest::DoRoleSwitchSuccessL()
	{
	LOG1(_L("RemDevState[%x]: <CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest> - role switch succeeded"), &StateMachine());

  	// tell the remote device the connection was successful
	StateMachine().SendRoleResponse(EOperationSuccessful);

	// Store the new roles
	StateMachine().LocalRole() = iPendingLocalRole;
	StateMachine().RemoteRole() = iPendingRemoteRole;

	// Changing role from GN/NAP to U.  If this connection previously had access to the
	// uplink this should be revoked.
	if(StateMachine().LocalRole() == EPanRoleU)
		{
		StateMachine().SetUplinkAccessAllowed(EFalse);
		}

	// let the role state machine know that this connection is now active
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateIncomingNotification(StateMachine());
	return(nextState);
	}
	
CPanRemDevStateBase* CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest::DoRoleSwitchFailureL(TInt /*aError*/)	
/**
Signal the remote device that the role change didn't work, and go back to active state
*/
	{
	LOG1(_L("RemDevState[%x]: <waiting for role switch for incoming role change request> - role change failed, signalling remote device and going to active state"), &StateMachine());
  	// tell the remote device the connection was unsuccessful
	StateMachine().SendRoleResponse(EConnectionNotAllowed);
	// let the role state machine know that the role change failed
	StateMachine().RoleChangeFailed();
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateActive(StateMachine());
	return(nextState);
	}

//
// CPanRemDevStateActive
//

CPanRemDevStateActive::CPanRemDevStateActive(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateActive)
/**

*/
	{	}
	
void CPanRemDevStateActive::OnEntryL()
/**

*/ 
	{
	LOG1(_L("RemDevState[%x]: <device active> - entering state"), &StateMachine());
	
	StateMachine().DeviceActive();
	}
	
void CPanRemDevStateActive::BnepRoleRequestFromRemoteDeviceL(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole)
/**
Remote device is trying to renegotiate roles with us
@note This may also occur because our role response got lost, so send it again...
*/
	{
	LOG1(_L("RemDevState[%x]: <device active> - BNEP role request received from remote device..."), &StateMachine());

	TInt err;
	TBluetoothPanRole proposedLocalRole;
	TBluetoothPanRole proposedRemoteRole;
	
	// Convert the role from the remote device.  If these are invalid the connection will be terminated.
	ConvertUuidsToPanRolesL(aRequestedLocalRole, aRequestedRemoteRole, proposedLocalRole, proposedRemoteRole);

	if((StateMachine().LocalRole() == proposedLocalRole) &&	(StateMachine().RemoteRole() == proposedRemoteRole)) // they've lost our response, so resend
		{
		LOG1(_L("RemDevState[%x]: <device active> - ...roles are the same as before, they obviously lost our role response - resending"), &StateMachine());
		StateMachine().SendRoleResponse(EOperationSuccessful);
		}
	else	// it's an attempt to renegotiate roles
		{
		LOG1(_L("RemDevState[%x]: <device active> - ..., they want to renegotiate roles"), &StateMachine());

		CPanRemDevStateBase* nextState = NULL;

		LOG1(_L("RemDevState[%x]: <active> - checking role request for incoming role upgrade attempt..."), &StateMachine());
		err = StateMachine().RoleChangeRequestFromPeer(proposedLocalRole, proposedRemoteRole);
		switch(err)
			{
			case KErrNone:
				{
				LOG1(_L("RemDevState[%x]: <active> - ...successful, checking to see if M/S switch necessary..."), &StateMachine());

				err = PerformMasterSlaveSwitchIfNecessary(proposedLocalRole);
				if(err==KErrWaitingForBasebandRoleSwitch)
					{
					nextState = new(ELeave) CPanRemDevStateWaitingForRoleSwitchForIncomingRoleChangeRequest(StateMachine(), proposedLocalRole, proposedRemoteRole);
					break;
					}
				else if(err)
					{
					LOG1(_L("RemDevState[%x]: <active> - ...M/S switch required and failed - disconnecting device."), &StateMachine());
					// remote device would not perform role switch (maybe it needs to stay as master for another profile it's using...)
					StateMachine().SendRoleResponse(EConnectionNotAllowed);
					StateMachine().RoleChangeFailed();
					break;		
					}
					
				LOG1(_L("RemDevState[%x]: <active> - ...M/S switch not required, sending successful connect response."), &StateMachine());
				StateMachine().SendRoleResponse(EOperationSuccessful);

				// Store the new roles
				StateMachine().LocalRole() = proposedLocalRole;
				StateMachine().RemoteRole() = proposedRemoteRole;

				// Changing role from GN/NAP to U.  If this connection previously had access to the
				// uplink this should be revoked.
				if(StateMachine().LocalRole() == EPanRoleU)
					{
					StateMachine().SetUplinkAccessAllowed(EFalse);
					}

				// remain in active state
				StateMachine().DeviceActive();
				break;
				}
			case KErrLocked:
				{
				LOG1(_L("RemDevState[%x]: <active> - ...role state machine busy, waiting."), &StateMachine());

				// Create the "waiting" state
				nextState = new(ELeave) CPanRemDevStateWaitingForLocalRoleStateMachineAvailabilityForIncomingRoleChangeRequest(StateMachine(), proposedLocalRole, proposedRemoteRole);
				break;
				}

			case KErrInvalidOrUnacceptableRoleCombination:
				LOG1(_L("RemDevState[%x]: <active> - ...Role change request can't be processed."), &StateMachine());		
				StateMachine().SendRoleResponse(EConnectionNotAllowed);
				// We don't want to change state or inform the state machine as it returned the error.
				break;

			default:
				// if err is something other than KErrNoMemory that might indicate an internal error in the state
				// machine, so ASSERT_DEBUG.  We can probably carry on even if the error is unexpected, as leaving
				// will cause this connection to shutdown.
				__ASSERT_DEBUG(err == KErrNoMemory, PanicInState(EPanAgentRoleStateMachineReturnedUnexpectedErrorCode));
				User::Leave(err);
				break;
			};
		
		if(nextState)	// we've decided we need to go to another state to continue
			{
			StateMachine().SetState(*nextState);
			delete this;
			return;
			}
		}
	}
	
void CPanRemDevStateActive::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage aRoleResponseCode)
/**
Something's gone very wrong - what is the other device doing?
*/
	{
	LOG1(_L("RemDevState[%x]: <device active> - BNEP role response received from remote device, what are they up to?  We haven't got an outstanding role request (this might change after a refactoring of the remote device states though)"), &StateMachine());

	// for robustness sake, accept a successful role response, but disconnect them otherwise
	if(aRoleResponseCode!=EOperationSuccessful)
		{
		User::Leave(KErrDodgyResponseFromRemoteDevice);
		}
	}

void CPanRemDevStateActive::RemoteDeviceDisconnectL(TInt aError)
/**
Remote device has disconnected
@param aError The reason for the remote device disconnect
*/
	{
	LOG1(_L("RemDevState[%x]: <device active> - remote device disconnect received, they've obviously given up talking to us."), &StateMachine());

	StateMachine().ResetConnectionRetryAttempts(); // reset the connection retry attempts

	User::Leave(aError); // will shutdown the connection	
	}
	
void CPanRemDevStateActive::ReadyForRoleRequestL()
/**
Indication that the main state machine wants us to renegotiate our roles
@note According to the spec, if we fail to negotiate a new role we remain connected and in our old roles
*/
	{
	LOG1(_L("RemDevState[%x]: <device active> - ready for role request received from role state machine - it obviously wants us to renegotiate our roles."), &StateMachine());	

	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingRoleNegotiationForOutgoingRoleChange(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanRemDevStateActive::ShutdownL()
/**
Disconnect command from local device
*/
	{
	LOG1(_L("RemDevState[%x]: <device active> - locally requested disconnect received."), &StateMachine());

	User::Leave(KErrLocallyInitiatedDisconnect); // will shutdown the connection
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateActive::GetState() const
	{
	return EActive;
	}
		
//
// CPanRemDevStateShutdown
//

CPanRemDevStateShutdown::CPanRemDevStateShutdown(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateShutdown)
/**

*/
	{	}
	
void CPanRemDevStateShutdown::OnEntryL()
/**

*/
	{
	LOG1(_L("RemDevState[%x]: <shutting down connection> - entering state"), &StateMachine());
	// put an async break in here to ensure that if we arrive here synchronously from
	// the initialising state in the role state machine, that we give the role state
	// machine chance to transition to a new state before telling it we're shutting down
	TRequestStatus* status = &StateMachine().Status();
	User::RequestComplete(status, KErrNone);
	StateMachine().SetActive();
	}

void CPanRemDevStateShutdown::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Shouldn't receive this (the remote device should be disconnected) but just in case...
@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	LOG1(_L("RemDevState[%x]: <shutting down connection> - BNEP role request received from remote device, but we're shutting down so just ignore it."), &StateMachine());

	// better tell them that we're not going to do anything
	StateMachine().SendRoleResponse(EConnectionNotAllowed);
	}
	
void CPanRemDevStateShutdown::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Shouldn't receive this (the remote device should be disconnected) but just in case...
@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	LOG1(_L("RemDevState[%x]: <shutting down connection> - BNEP role response received from remote device, but we're shutting down so just ignore it."), &StateMachine());

	// We're disconnecting, so we don't care, just bin it (ie. do nothing...)
	}
	
void CPanRemDevStateShutdown::RemoteDeviceDisconnectL(TInt /*aError*/)
/**
Shouldn't receive this (the remote device should be disconnected) but just in case...
@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	LOG1(_L("RemDevState[%x]: <shutting down connection> - remote device disconnect received; odd, they should already be disconnected."), &StateMachine());

	// Again, we're disconnecting, so we don't care, just bin it (ie. do nothing...)
	}
	
void CPanRemDevStateShutdown::ShutdownL()
/**
Whoops - double disconnect from local device...
@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	// It is possible to request a shutdown of a remote device when it is already in
	// the shutdown state, so don't panic. This occurs when the last remote device receives a
	// disconnect and places itself in the shutdown state but only removes itself from the agent 
	// after an async event.
	// As it was the last remote device this cause LinkLayerDown() to be called on Nifman which
	// shuts down the agent. The agent then disconnects all remote devices. If the async event
	// has not occured for the last remote device shutdown then the agent calls shutdown on the 
	// remote device again. This is ok as the async event completing will inform the agent and then
	// the agent will do a full shutdown.

	LOG1(_L("RemDevState[%x]: <shutting down connection> - remote device disconnect request received, already shutting down so nothing to do."), &StateMachine());
	}
	
void CPanRemDevStateShutdown::AsyncEventCompleteL()
/**
Async callback to tell us to shutdown the state machine
@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	LOG1(_L("RemDevState[%x]: <shutting down connection> - async event complete, shutting down"), &StateMachine());
	
	if(StateMachine().HasBnepChannel())
		{
		StateMachine().DisconnectBnepChannel();
		}
	
	StateMachine().ShutdownComplete();
	// don't "delete this" - this is the terminal state, so the state machine has to delete us	
	}
	
void CPanRemDevStateShutdown::AsyncEventCancelL()
/**

@note *Must not* leave from here, otherwise we're in an endless loop
*/
	{
	TRequestStatus* status = &StateMachine().Status();
	User::RequestComplete(status, KErrCancel); // to keep CActive::Cancel() happy
	}

// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateShutdown::GetState() const
	{
	return EDisconnecting;
	}

//
// CPanRemDevStateIncomingNotification
//

CPanRemDevStateIncomingNotification::CPanRemDevStateIncomingNotification(MPanRemDevStateMachineNotify& aStateMachine) :
	CPanRemDevStateBase(aStateMachine, EPanRemDevStateIncomingNotification)
/**
This state manages the incoming uplink access allowed for local NAP role notifier
*/
	{	}
	
CPanRemDevStateIncomingNotification::~CPanRemDevStateIncomingNotification()
	{
	delete iNapUplinkAuthorisationHelper;
	}
	
void CPanRemDevStateIncomingNotification::OnEntryL()
	{
	if(StateMachine().LocalRole() == EPanRoleNap)
		{
		iNapUplinkAuthorisationHelper = new(ELeave) CNapUplinkAuthorisationHelper(*this);
		
		// Set requested connection address.
		TBTDevAddr requestingDeviceAddr = StateMachine().RemSockAddr().BTAddr();		
		iNapUplinkAuthorisationHelper->PanConnectionList().SetRequestedConnectionAddr(requestingDeviceAddr);

		// Get current connection information.
		StateMachine().GetExistingConnections(iNapUplinkAuthorisationHelper->PanConnectionList());
		
		iNapUplinkAuthorisationHelper->PerformNapUplinkAuthorisationL();
		}
	else
		{
		// Notifier not required - go to active state.
		CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStateActive(StateMachine());		
		StateMachine().SetState(*nextState);
		delete this;
		return;		// Just to be safe after 'delete this'
		}
	}

// From MNapUplinkAuthorisationRequester
void CPanRemDevStateIncomingNotification::MnuarNapUplinkAuthorisationHelperComplete(TInt aError, TNapConnectionResult aResult)
	{
	if(aError == KErrNone)
		{
		switch(aResult)
			{
			case EDisallowNewNapConnection:
				// Disconnect this connection by moving into the shutdown state.
				ShutdownConnection(KErrAccessDenied);
				break;
				
			case EAcceptNapConnectionAllowUplinkAccess:
				// Inform the packet driver that this connection should have access to the NAP service and
				// close any connection that currently has access to the uplink.
				StateMachine().SetUplinkAccessAllowed(ETrue);
		
				// Fall onto next case...
						
			case EAcceptNapConnectionDisallowUplinkAccess:
				{
				CPanRemDevStateBase* nextState = NULL;
				TRAPD(err, nextState = new(ELeave) CPanRemDevStateActive(StateMachine()));
				if(err == KErrNone)
					{
					StateMachine().SetState(*nextState);
					delete this;
					return;
					}
				else
					{
					ShutdownConnection(err);
					}
				}
				break;
				
			default:
				PanicInState(EInvalidResultCodeFromNapUplinkAuthorisationNotifier);
				break;
			};
		}
	else
		{
		// Disconnect this connection by moving into the shutdown state.
		ShutdownConnection(aError);
		}
	}
	
void CPanRemDevStateIncomingNotification::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
A role request from the remote device can't be handled at this time
*/
	{
	LOG1(_L("RemDevState[%x]: <Waiting for incoming notification> - BNEP role reponse from remote device."), &StateMachine());
	
	// drop the role request packet that the remote device has sent us
	}
	
void CPanRemDevStateIncomingNotification::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
A role request has not been sent by this device.  This is unexpected behaviour by the remote.
*/
	{
	LOG1(_L("RemDevState[%x]: <Waiting for incoming notification> - BNEP role reponse from remote device."), &StateMachine());
	
	// drop the role response packet that the remote device has incorrectly sent us
	}
	
void CPanRemDevStateIncomingNotification::RemoteDeviceDisconnectL(TInt aError)
	{
	LOG1(_L("RemDevState[%x]: <Waiting for incoming notification> - remote device disconnect received; odd, they should already be disconnected."), &StateMachine());

	// Shutdown the connection.
	ShutdownConnection(aError);
	}
	
void CPanRemDevStateIncomingNotification::ShutdownL()
	{
	LOG1(_L("RemDevState[%x]: <CPanRemDevStateIncomingNotification> - locally requested disconnect received"), &StateMachine());
	
	// outstanding async request will be cancelled by state machine when we leave
	User::Leave(KErrLocallyInitiatedDisconnect);
	}
	
// This returns a distilled (i.e., not the actual remote device state)
// version of the current connection status of the remote device
TRemoteDeviceState CPanRemDevStateIncomingNotification::GetState() const
	{
	return EPerformingNegotiation;
	}

void CPanRemDevStateIncomingNotification::ShutdownConnection(TInt aError)
	{
	if(!iShutdownCalled)
		{
		// Shutdown the connection.
		iShutdownCalled = ETrue;
		StateMachine().Shutdown(aError);
		}
	}

	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CPanRemDevStatePaused::CPanRemDevStatePaused(MPanRemDevStateMachineNotify& aStateMachine, CPanRemDevStateBase& aNextState)
    : CPanRemDevStateBase(aStateMachine, EPanRemDevStatePaused), iNextState(&aNextState)
    {
    }
    
CPanRemDevStatePaused::~CPanRemDevStatePaused()
    {
    delete iNextState;
    }

void CPanRemDevStatePaused::OnEntryL()
    {
    // Does nothing
    }
    
void CPanRemDevStatePaused::ShutdownL()
    {
    // We don't know the appropriate action so allow the next states
    // ShutdownL to execute. We'll cleanup the next state when the 
    // RemDevStateMachine deletes us
    iNextState->ShutdownL();
    }
    
TRemoteDeviceState CPanRemDevStatePaused::GetState() const
    {
    return iNextState->GetState();
    }

void CPanRemDevStatePaused::AsyncEventCancelL()
    {
    // Next state hasn't started yet so no need to cancel it
    }
    
void CPanRemDevStatePaused::AsyncEventCompleteL()
    {
    StateMachine().SetState(*iNextState);
    
    // We no longer own the next state
    iNextState = NULL;
    delete this;
    }
    
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
