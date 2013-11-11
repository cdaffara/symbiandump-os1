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
 @note PAN agent role state machine state implementations
*/

#include <bluetooth/logger.h>
#include "panagtstates.h"
#include "panagtremdev.h"
#include "panlocalsdphelper.h"
#include "BTSec.h"
using namespace PanAgent;

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

CPanAgtStateBase* CPanAgtStateUninitialised::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateUninitialised* self = new (ELeave) CPanAgtStateUninitialised(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CPanAgtStateUninitialised::OnEntryL()
	{	
	}

CPanAgtStateUninitialised::CPanAgtStateUninitialised(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgentStateUninitialised)
	{
	}
//
// CPanAgentStateInitialising
//
CPanAgentStateInitialising::CPanAgentStateInitialising(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgentStateInitialising)
/**

*/
	{	}
	
CPanAgtStateBase* CPanAgentStateInitialising::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgentStateInitialising* self = new (ELeave) CPanAgentStateInitialising(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CPanAgentStateInitialising::OnEntryL()
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(EActiveDevicesInArrayInInitialisingState));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(ENegotiatingDevicesInArrayInInitialisingState));

	LOG(_L("RoleState: <initialising> - entering state"));
	
	// Have a look in commdb to see if we have a defined local role
	TUint32 roleTemp;
	StateMachine().Database().GetIntL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_LOCAL_ROLE), roleTemp);
	__ASSERT_DEBUG(roleTemp!=0xffff, PanicInState(EOldFormatPanRoleBeingUsedInCommdb));
	StateMachine().FixedLocalRole() = static_cast<TBluetoothPanRole>(roleTemp);
	LOG1(_L("RoleState: <initialising> - local device state in commdb is %x"), StateMachine().FixedLocalRole());

	// same thing for remote role
	StateMachine().Database().GetIntL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_PEER_ROLE), roleTemp);
	__ASSERT_DEBUG(roleTemp!=0xffff, PanicInState(EOldFormatPanRoleBeingUsedInCommdb));
	StateMachine().FixedRemoteRole() = static_cast<TBluetoothPanRole>(roleTemp);
	LOG1(_L("RoleState: <initialising> - remote device state in commdb is %x"), StateMachine().FixedRemoteRole());

	// Check if PAN NAP is allowed as a local role.
	TBool napEnabled = EFalse;
	TRAP_IGNORE(StateMachine().Database().GetBoolL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_NAP_SERVICE_ENABLED), napEnabled));
	StateMachine().SetNapEnabled(napEnabled);	
	
	// Check for incoming connection support and enable if necessary
	TBool allowIncoming;
	StateMachine().Database().GetBoolL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_ALLOW_INCOMING), allowIncoming);
	StateMachine().SetAllowIncoming(allowIncoming);	

	// make sure the fixed roles (if there are any) make sense
	User::LeaveIfError(IsRoleRequestValid(StateMachine().FixedLocalRole(), StateMachine().FixedRemoteRole()));

	if(napEnabled)
		{
		// NAP can only be enabled for a listening IAP.
		if(!allowIncoming) 
			{
			LOG(_L("RoleState: <initialising> - Listening must be enabled when NAP is enabled.  Fail initialisation"));
			User::Leave(KErrArgument);
			}
			
		if(!(StateMachine().FixedLocalRole() == EPanRoleUnknown || StateMachine().FixedLocalRole() == EPanRoleNap))
			{
			LOG1(_L("RoleState: <initialising> - Invalid Fixed Local Role %x"), StateMachine().FixedLocalRole());
			User::Leave(KErrArgument);
			}
		}
	else
		{
		if(StateMachine().FixedLocalRole() == EPanRoleNap)
			{
			LOG(_L("RoleState: <initialising> - Invalid Fixed Local Role PAN-NAP when NAP not enabled"));
			User::Leave(KErrArgument);
			}
		}
		
	if(allowIncoming)
		{
		// Only any point in advertising in SDP if we're allowing incoming connections...
		LOG(_L("RoleState: <initialising> - incoming support is enabled"));

		if(StateMachine().FixedLocalRole() == EPanRoleU)
			{
			LOG(_L("RoleState: <initialising> - Invalid PAN-U Fixed Local Role for listening IAP"));
			User::Leave(KErrArgument);
			}

		if(!(StateMachine().FixedRemoteRole() == EPanRoleUnknown || StateMachine().FixedRemoteRole() == EPanRoleU))
			{
			LOG1(_L("RoleState: <initialising> - Invalid Fixed Remote Role for listening IAP %x"), StateMachine().FixedRemoteRole());
			User::Leave(KErrArgument);
			}
		
		//If other services are active currently, we will not register sdp records and open listen socket
		if (StateMachine().IsNewPanConnectionAllowed())
			{
			StartListenerL();
			}
		}
#ifdef _DEBUG
	else  	// no incoming support, so just start outgoing connections and move to idle state
			// if incoming support is enabled, this will be done after all the SDP roles are registered
		{
		LOG(_L("RoleState: <initialising> - incoming support is disabled"));
		}
#endif		
	DoStartOutgoingL();
	}
		
void CPanAgentStateInitialising::DoStartOutgoingL()
/**
Start setting up outgoing connections, and move to the idle state
*/
	{
	if(StateMachine().SuppressOutgoing())	// check that outgoing connections are not suppressed 
											// (to allow UIs to start the ad-hoc IAP in incoming mode only,
											// and to allow reuse of the registering SDP records state to 
											// perform reregistration of SDP records - when reregistering
											// records, we don't want to prompt to start an outgoing connection)
		{
		LOG(_L("RoleState: <initialising/registering local SDP records> - outgoing connections suppressed, resetting suppression flag and moving directly to idle state"));
		StateMachine().ResetSuppressOutgoing(); // stop future outgoing connections being suppressed
		if(!(StateMachine().IsIncomingConnectionListenerEnabled()))
			{
			LOG(_L("RoleState: <initialising/registering local SDP records> - This is an invalid state while outgoing connection is suppressed without incoming listening enabled."));
			// outstanding async request will be cancelled by state machine when we leave
			User::Leave(KErrListenForIncomingConnectionRequestedWithoutListeningSupport);
			}
		}
	else
		{
		// outstanding async request will be cancelled by state machine when we leave
		StateMachine().SetupOutgoingConnectionsL();
		}
	
	CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());			
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

//
// CPanAgtStateIdle
//
CPanAgtStateBase* CPanAgtStateIdle::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateIdle* self = new (ELeave) CPanAgtStateIdle(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateIdle::CPanAgtStateIdle(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateIdle)
/**

*/
	{	}
	
void CPanAgtStateIdle::OnEntryL()
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(EActiveDevicesInArrayInInitialisingState));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(ENegotiatingDevicesInArrayInInitialisingState));

	LOG(_L("RoleState: <idle> - entering state"));

	StateMachine().SendProgress(EPanAgtIdle, KErrNone);
	StartNegotiationWithFirstPendingDevice(); // if there's not already a device negotiating, start one negotiating
	}
	
void CPanAgtStateIdle::InitiateOutgoingConnectionL(TBluetoothPanRole& aRequestedLocalRole, TBluetoothPanRole& aRequestedRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles)
/**

*/
	{	
	LOG(_L("RoleState: <idle> - InitiateOutgoingConnectionL"));
	LOG3(_L("RoleState: <idle> - ...worthTrying roles - U:%x GN:%x NAP:%x..."), aWorthTryingRemoteRoles.IsWorthTryingU(), aWorthTryingRemoteRoles.IsWorthTryingGn(), aWorthTryingRemoteRoles.IsWorthTryingNap());
	
	// Determine the roles for this outgoing connection.
	aRequestedLocalRole = StateMachine().FixedLocalRole(); 
	if(aRequestedLocalRole == EPanRoleUnknown)
		{
		aRequestedLocalRole = EPanRoleU;
		}

	// aRequestedLocalRole should be set to the local role we want to be by the time we call the method below
	User::LeaveIfError(SelectRemoteRoleBasedOnLocalRole(aRequestedLocalRole, aRequestedRemoteRole, aWorthTryingRemoteRoles));

	// and now transition to the appropriate state
	CPanAgtStateBase* nextState = NULL;
	switch(aRequestedLocalRole)
		{
		case EPanRoleU:
			{
			// What about a multiple start case where there are several devices started at once,
			// you may be asking?
			// This is the wrong state to go into then...  However, if they all arrive in the 
			// outgoing connection starter at the same time (which they will), then they'll all 
			// get dumped into the array sychronously, and since each one has to asyhronously
			// connect a socket, by the time they call us for their role request, we'll know 
			// there's >1 device
			nextState = CPanAgtStateNegotiatingURole::NewL(StateMachine());
			break;			
			}
		case EPanRoleGn:
		case EPanRoleNap:
			{
			nextState = CPanAgtStateNegotiatingGnOrNapRole::NewL(StateMachine());
			break;
			}

		default:
			{
			PanicInState(EPanAgentRoleStateMachineInvalidLocalRole);
			break;
			}
		}

	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	}

void CPanAgtStateIdle::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**
A remote device has disconnected before it even began role negotiation!  Obviously something dodgy about their role request...
*/
	{
	DoDeviceDisconnectL(aDevice);
	}

void CPanAgtStateIdle::IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole)
	{
	CPanAgtStateBase* nextState = NULL;
	switch(aLocalRole)
		{
		case EPanRoleU:
			{
			nextState = CPanAgtStateNegotiatingURole::NewL(StateMachine());
			break;			
			}

		case EPanRoleGn:
		case EPanRoleNap:
			{
			nextState = CPanAgtStateNegotiatingGnOrNapRole::NewL(StateMachine());
			break;
			}

		default:
			PanicInState(EInvalidRoleForIncomingConnectionFromPeerL);
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	}

//
// CPanAgtStateNegotiatingURole
//
CPanAgtStateBase* CPanAgtStateNegotiatingURole::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateNegotiatingURole* self = new (ELeave) CPanAgtStateNegotiatingURole(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateNegotiatingURole::CPanAgtStateNegotiatingURole(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateNegotiatingURole)
/**

*/
	{	}

CPanAgtStateNegotiatingURole::CPanAgtStateNegotiatingURole(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber) :
	CPanAgtStateBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}
	
void CPanAgtStateNegotiatingURole::OnEntryL()
/**
Start deregistering unwanted local SDP records
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(EActiveDevicesInArrayInNegotiatingPanUState));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==1, PanicInState(EMoreThanOneDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <negotiating U> - entering state"));
		
	if(StateMachine().AllowIncoming())
		{
		// stop advertising U record
		StateMachine().PanLocalSdpRegistrar().UnregisterLocalSdpRecord(EPanRoleU);
		}
	}
	
void CPanAgtStateNegotiatingURole::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aRequestedLocalRole*/, TBluetoothPanRole& /*aRequestedRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
/**
Don't accept more role requests when we're negotiating
*/
	{
	LOG(_L("RoleState: <negotiating U> - InitiateOutgoingConnectionL to another device, returning busy"));
	User::Leave(KErrLocked);
	}

void CPanAgtStateNegotiatingURole::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	LOG(_L("RoleState: <negotiating U> - IncomingConnectionFromPeerL , returning busy"));
	User::Leave(KErrLocked);
	}
	
void CPanAgtStateNegotiatingURole::DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice)
/**
A device has finished negotiation and is now active
*/
	{
	__ASSERT_ALWAYS(FindDeviceInArray(aDevice)>=0, PanicInState(EDeviceAttemptingToNegotiateThatIsNotInArray));
	__ASSERT_DEBUG(aDevice.GetState() == EActive, PanicInState(EDeviceActiveCalledByDeviceNotInActiveState));

	CPanAgtStateBase* nextState = CPanAgtStateURole::NewL(StateMachine());		
	StateMachine().SetState(*nextState);
	delete this;
	}

void CPanAgtStateNegotiatingURole::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**
A device has disconnected
*/
	{
	//This function will only be called when device is in EDisconnecting State.
	//Remote device state machine will first shutdown itself and then call panagent to
	//disconnect this device and remove it from the device array
	__ASSERT_DEBUG(aDevice.GetState() == EDisconnecting, PanicInState(EInvalidRemoteDeviceState));
	
	// disconnect them and shutdown or return to the idle state
	// outstanding async request will be cancelled by state machine when we leave
	DoDeviceDisconnectL(aDevice); // will leave and shutdown the connection if necessary

		StartListenerL();

	CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

//
// CPanAgtStateURole
//
CPanAgtStateBase* CPanAgtStateURole::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateURole* self = new (ELeave) CPanAgtStateURole(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateURole::CPanAgtStateURole(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateURole)
/**

*/
	{	}
	
void CPanAgtStateURole::OnEntryL()
/**

*/
	{
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0,PanicInState(EDeviceInNegotiatingModeOnEntryToUState));
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ETooManyDevicesAttachedInURole));

	LOG(_L("RoleState: <U role> - entering state"));

	StateMachine().SendProgress(EPanAgtURole, KErrNone);
	StartNegotiationWithFirstPendingDevice();
	}
	
void CPanAgtStateURole::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aRequestedLocalRole*/, TBluetoothPanRole& /*aRequestedRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
	{
	// Need to become GN / NAP.  Check that this will be acceptable for the fixed roles
	if(StateMachine().FixedLocalRole() == EPanRoleU || 
	   (StateMachine().FixedRemoteRole() == EPanRoleNap || StateMachine().FixedRemoteRole() == EPanRoleGn))
		{
		User::Leave(KErrInvalidOrUnacceptableRoleCombination);
		}
	else
		{
		CPanAgtStateBase* nextState = CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::NewL(StateMachine());
		StateMachine().SetState(*nextState);
		delete this;
		User::Leave(KErrLocked);
		}
	}

void CPanAgtStateURole::IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole)
	{
	if(aLocalRole == EPanRoleU)
		{
		// An existing connection is established to the PAN-U role.
		User::Leave(KErrInvalidOrUnacceptableRoleCombination);
		}

	// Start upgrade to GN/NAP
	CPanAgtStateBase* nextState = CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	User::Leave(KErrLocked); // hold the new device off until we've renegotiated with the existing one			
	}

void CPanAgtStateURole::RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole)
	{
	CPanAgtStateBase* nextState = NULL;
	switch(aLocalRole)
		{
		case EPanRoleU:
			nextState = CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::NewL(StateMachine());
			break;
			
		case EPanRoleGn:
		case EPanRoleNap:
			nextState = CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::NewL(StateMachine());
			break;
			
		default:
			PanicInState(EInvalidLocalRoleFromRoleChangeRequestFromPeer);
		}
		
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	}
	
#ifdef _DEBUG
void CPanAgtStateURole::DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice)
#else
void CPanAgtStateURole::DeviceActiveL(CPanRemoteDeviceStateMachine& /*aDevice*/)
#endif // _DEBUG
/**
@note This is only allowed if it's the same device as we're already attached to 
ie. we're performing a role switch
*/
	{
	__ASSERT_DEBUG(aDevice.GetState() == EPerformingNegotiation, PanicInState(EUnexpectedDeviceActiveReceivedByState));
	}

void CPanAgtStateURole::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**
A remote device has completed its shutdown
*/
	{
	LOG(_L("RoleState: <U role> - device disconnected..."));
	DoDeviceDisconnectL(aDevice); // if there are no devices in the array and we're not supporting incoming connection this method will leave

	// This could be the on device connected to the U or it could be a failed upgrade device.
	if(StateMachine().Devices().Count() == 0)
		{
		StartListenerL();
		CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());
		StateMachine().SetState(*nextState);
		delete this;
		}
	}

void CPanAgtStateURole::StateSendRoleNotification()
	{
	StateMachine().SendProgress(EPanAgtURole, KErrNone);
	}
		
//
// CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest
//
CPanAgtStateBase* CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest* self = new (ELeave) CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest)
/**

*/
	{	}
	
void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::OnEntryL()
/**
@pre Just come from U mode, so only one active device in the array
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ETooManyDevicesAttachedWhenRenegotiatingActiveConnectionForGnOrNapUpgrade)); // not quite enough to ensure we were previously in U role, but as good as we're going to get

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - entering state"));
	
	StateMachine().SendProgress(EPanAgtReconfiguringPiconet, KErrNone);
	
	CPanRemoteDeviceStateMachine& currentActiveDevice = FindDeviceInActiveState(); // there will only be one (we're in U role)
	currentActiveDevice.ReadyForRoleRequest(); // signal it to redo role negotiation
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aRequestedLocalRole*/, TBluetoothPanRole& /*aRequestedRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::RoleChangeRequestFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	// Not in a state to handle this unexpected event.
	User::Leave(KErrNotReady);
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::PerformLocalRoleChangeRequestL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole)
	{
	aLocalRole = StateMachine().NapEnabled() ? EPanRoleNap : EPanRoleGn;
	aRemoteRole = EPanRoleU;
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice)
/**
We've successfully managed to renegotiate roles with the currently connected device
*/
	{
	__ASSERT_ALWAYS(FindDeviceInArray(aDevice)>=0, PanicInState(EDeviceAttemptingToNegotiateThatIsNotInArray));
	__ASSERT_DEBUG(aDevice.GetState() == EActive, PanicInState(EDeviceActiveCalledByDeviceNotInActiveState));
	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - device active received"));

	CPanAgtStateBase* nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Role upgrade failed, return to our previous state
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(EActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtOurBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==1, PanicInState(EMoreThanOneDeviceInNegotiatingModeInArray));
	
	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - role change failed..."));
	// there should be (at least one) pending device (the one that caused us to attempt the upgrade to GN/NAP)
	// disconnect it (them), because the role change failed
	// need to do this because the new device may have been added by the user, so we can't rely
	// on a timeout from a remote device, because there may not be one connected
	RPointerArray<CPanRemoteDeviceStateMachine>& deviceArray = StateMachine().Devices();
	for(TInt i = 0; i < deviceArray.Count(); ++i)
		{
		if(deviceArray[i]->GetState() != EPerformingNegotiation)	// ie. it's not the device we just failed to get to change role
			{
			LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - ...disconnecting non-negotiating device..."));
			deviceArray[i]->Disconnect();
			}
		}
		
	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - ...setting original device active again and going to U role"));

	// since we're negotiating GN/NAP role, then we must previously have been in U role
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(ETooManyDevicesAttachedWhenRenegotiatingActiveConnectionForGnOrNapUpgrade)); // not quite enough to ensure we were previously in U role, but as good as we're going to get
	__ASSERT_DEBUG(NegotiatingDeviceCount()==1, PanicInState(EMoreThanOneDeviceInNegotiatingModeInArray));

	// Enter negotiating U role state and wait for the device active 
	// notification from the remote device state machine
	CPanAgtStateBase* nextState = CPanAgtStateNegotiatingURole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==0, PanicInState(EActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtOurBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==1, PanicInState(EMoreThanOneDeviceInNegotiatingModeInArray));


	//This function will only be called when device is in EDisconnecting State.
	//Remote device state machine will first shutdown itself and then call panagent to
	//disconnect this device and remove it from the device array
	__ASSERT_DEBUG(aDevice.GetState() == EDisconnecting, PanicInState(EInvalidRemoteDeviceState));

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest> - whoops, the remote device we're trying to upgrade has disconnected!"));
	DoDeviceDisconnectL(aDevice); // will leave if there are no other devices waiting and we're not accepting incoming connections

	// go back to idle state and start negotiating with the idle device that should be in the array
	// there should be one there, as it was the reason that we tried to do role upgrade in the first place!
	StartListenerL();

	CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;

	}

CPanRemoteDeviceStateMachine& CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtOurBehest::FindDeviceInActiveState()
/**
Return the device that is in the active state
@pre There is only one device in an active state
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(EMoreThanOneActiveDeviceWhenTryingToRenegotiateUToGnOrNapRoleChange));

	RPointerArray<CPanRemoteDeviceStateMachine>& deviceArray = StateMachine().Devices();
	for(TInt i=0;i<deviceArray.Count();++i)
		{
		if(deviceArray[i]->GetState() == EActive)
			{
			return (*(deviceArray[i]));
			}
		}
		
	PanicInState(ENoActiveDeviceInArray);
	// next bit will never be reached, but it keeps the compiler quiet...
	CPanRemoteDeviceStateMachine* notUsed = NULL;
	return(*notUsed);
	}

//
// CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest
//
CPanAgtStateBase* CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest* self = new (ELeave) CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest)
/**

*/
	{	}
	
void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::OnEntryL()
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDeviceInArrayInRenegActConnToURoleAtPeersBehest)); 		// if we're going to U and we've not got an active device, we're in trouble
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));
	LOG(_L("RoleState: <renegotiating active connection to U role at peers behest> - entering state"));

	StateMachine().SendProgress(EPanAgtReconfiguringPiconet, KErrNone);	
	
	// we've already had the role request from the remote device in the previous state, so just
	// wait for device active or role changed failed
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aRequestedLocalRole*/, TBluetoothPanRole& /*aRequestedRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}

void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice)
/**
We've successfully managed to renegotiate roles with the currenly connected device
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(EActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtOurBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <renegotiating active connection to U role at peers behest> - device active, role change successful"));

	if(aDevice.GetState() == EActive) // we've succeeded in changing role
		{
		CPanAgtStateBase* nextState = CPanAgtStateURole::NewL(StateMachine());
		StateMachine().SetState(*nextState);
		delete this;
		return;
		}
	else
		{
		PanicInState(EUnexpectedDeviceActiveReceivedByState);
		}
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Unlikey this can happen - if we get into this state we've decided we can do the role change.
If this gets called, the remote device state machine is probably doing something dodgy
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDeviceInArrayInRenegActConnToURoleAtPeersBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <renegotiating active connection to U role at peers behest> - role changed failed - strange, the peer must have asked us to go to U role, but for some reason we decided we could, then it turned out we couldn't.  Sounds like remote device state machine is doing something dodgy"));

	// we couldn't go to U role (unlikely this can happen) so go back to GN/NAP state
	CPanAgtStateBase* nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDeviceInArrayInRenegActConnToURoleAtPeersBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <renegotiating active connection to U role at peers behest> - whoops, the remote device we're trying to upgrade has disconnected!"));
	DoDeviceDisconnectL(aDevice); // will leave if there are no other devices waiting and we're not accepting incoming connections

	// return to idle (possibly after reregistering local SDP record) - there can't be
	// any active devices left attached
	StartListenerL();

	CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}
	
//
// CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest
//
CPanAgtStateBase* CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest* self = new (ELeave) CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest)
/**

*/
	{	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::OnEntryL()
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtPeersBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest> - entering state"));
	
	StateMachine().SendProgress(EPanAgtReconfiguringPiconet, KErrNone);
	
	// we've already had the role request from the remote device in the previous state, so just
	// wait for device active or role changed failed	
	}


void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aLocalRole*/, TBluetoothPanRole& /*aRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}
	
void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::DeviceActiveL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtPeersBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest> - device active received from negotitaing device"));	

	CPanAgtStateBase* nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Unlikey this can happen - if we get into this state we've decided we can do the role change.
If this gets called, the remote device state machine is probably doing something dodgy
*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(ENoActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtPeersBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest> - role changed failed - strange, the peer must have asked us to go to U role, but for some reason we decided we could, then it turned out we couldn't.  Sounds like remote device state machine is doing something dodgy"));

	// we couldn't go to GN/NAP role (unlikely this can happen) so go back to U role
	CPanAgtStateBase* nextState = CPanAgtStateURole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

void CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()==1, PanicInState(EActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtOurBehest));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EUnexpectedDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <CPanAgtStateRenegotiatingActiveConnectionToGnOrNapRoleAtPeersBehest> - whoops, the remote device we're trying to upgrade has disconnected!"));
	DoDeviceDisconnectL(aDevice); // will leave if there are no other devices waiting and we're not accepting incoming connections

	StartListenerL();

	CPanAgtStateBase* nextState = CPanAgtStateIdle::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	return;
	}

//
// CPanAgtStateNegotiatingGnOrNapRole
//
CPanAgtStateBase* CPanAgtStateNegotiatingGnOrNapRole::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateNegotiatingGnOrNapRole* self = new (ELeave) CPanAgtStateNegotiatingGnOrNapRole(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateNegotiatingGnOrNapRole::CPanAgtStateNegotiatingGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateNegotiatingGnOrNapRole)
/**

*/
	{	}

CPanAgtStateNegotiatingGnOrNapRole::CPanAgtStateNegotiatingGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber) :
	CPanAgtStateBase(aStateMachine, aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{	}

void CPanAgtStateNegotiatingGnOrNapRole::OnEntryL()
/**
@note Remote device object will make us piconet master
*/
	{
	LOG(_L("RoleState: <CPanAgtStateNegotiatingGnOrNapRole> - entering state"));

	if(StateMachine().AllowIncoming()) // stop advertising U service record
		{
		StateMachine().PanLocalSdpRegistrar().UnregisterLocalSdpRecord(EPanRoleU);
		}
	}

void CPanAgtStateNegotiatingGnOrNapRole::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aLocalRole*/, TBluetoothPanRole& /*aRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}
	
void CPanAgtStateNegotiatingGnOrNapRole::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
	{
	// Negotiation in progress - Wait...
	User::Leave(KErrLocked);
	}

	
void CPanAgtStateNegotiatingGnOrNapRole::DeviceActiveL(CPanRemoteDeviceStateMachine& aDevice)
/**
We've completed BNEP negotiation with the remote device successfully
*/
	{
	__ASSERT_ALWAYS(FindDeviceInArray(aDevice)>=0, PanicInState(EActiveDeviceThatIsNotInArray));
	__ASSERT_DEBUG(aDevice.GetState() == EActive, PanicInState(EDeviceNotActive));

	LOG(_L("RoleState: <CPanAgtStateNegotiatingGnOrNapRole> - device active received..."));

	CPanAgtStateBase* nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());		
	StateMachine().SetState(*nextState);
	delete this;
	}

void CPanAgtStateNegotiatingGnOrNapRole::DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Looks like the remote device asked us to act in a role that we couldn't support
@note This can only be called as part of a role change, not during initial negotiation with a device
*/
	{
	__ASSERT_DEBUG(NegotiatingDeviceCount()==1, PanicInState(EMoreThanOneDeviceInNegotiatingModeInArray));

	LOG(_L("RoleState: <CPanAgtStateNegotiatingGnOrNapRole> - role changed failed - this *might* be triggered by an incoming connection from a remote device asking us to be PAN-U, and we can't because we've got other attached devices"));

	CPanAgtStateBase* nextState = NULL;
	nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());		
	
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
	StateMachine().SetState(*nextState);
	delete this;
	}
	
void CPanAgtStateNegotiatingGnOrNapRole::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**

*/
	{

	//This function will only be called when device is in EDisconnecting State.
	//Remote device state machine will first shutdown itself and then call panagent to
	//disconnect this device and remove it from the device array
	__ASSERT_DEBUG(aDevice.GetState() == EDisconnecting, PanicInState(EInvalidRemoteDeviceState));

	LOG(_L("RoleState: <CPanAgtStateNegotiatingGnOrNapRole> - oops, the remote device we're negotiating with has disconnected"));
	// outstanding async request will be cancelled by state machine when we leave
	DoDeviceDisconnectL(aDevice); // will leave if there are no more devices connected and we're not accepting incoming connections

	CPanAgtStateBase* nextState = NULL;
	if(ActiveDeviceCount()>=1)
		{
		nextState = CPanAgtStateGnOrNapRole::NewL(StateMachine());
		}
	else
		{
	    StartListenerL();
		nextState = CPanAgtStateIdle::NewL(StateMachine()); // will trigger negotiation with idle device
		}
	__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));		
	StateMachine().SetState(*nextState);
	delete this;
	return;
	
	}

	
//
// CPanAgtStateGnOrNapRole
//
CPanAgtStateBase* CPanAgtStateGnOrNapRole::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateGnOrNapRole* self = new (ELeave) CPanAgtStateGnOrNapRole(aStateMachine);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateGnOrNapRole::CPanAgtStateGnOrNapRole(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateGnOrNapRole)
/**

*/
	{	}

void CPanAgtStateGnOrNapRole::OnEntryL()	
/**

*/
	{
	__ASSERT_DEBUG(ActiveDeviceCount()>=1, PanicInState(ENoActiveDeviceAfterNegotiationCompleted));
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(EDeviceInNegotiatingModeOnEntryToGnOrNapState));

	LOG1(_L("RoleState: <CPanAgtStateGnOrNapRole> - entering state, %d active devices"), ActiveDeviceCount());

	if(StateMachine().NapEnabled())
		{
		StateMachine().SendProgress(EPanAgtNapRole, KErrNone);
		}
	else
		{
		StateMachine().SendProgress(EPanAgtGnRole, KErrNone);
		}
		
	LockDeviceAsPiconetMasterL(); // apply piconet master lock each time we enter this state, to ensure all devices have this set

	// scan the array to see if there are any devices awaiting negotiation, if so, trigger negotiation with one of them
	StartNegotiationWithFirstPendingDevice();
	}
	
void CPanAgtStateGnOrNapRole::IncomingConnectionFromPeerL(TBluetoothPanRole aLocalRole)
	{
	if(StateMachine().NapEnabled())
		{
		if(aLocalRole != EPanRoleNap)
			{
			User::Leave(KErrInvalidOrUnacceptableRoleCombination);
			}
		}
	else
		{
		if(aLocalRole != EPanRoleGn)
			{
			User::Leave(KErrInvalidOrUnacceptableRoleCombination);
			}
		}
	}
	
void CPanAgtStateGnOrNapRole::RoleChangeRequestFromPeerL(TBluetoothPanRole aLocalRole)
	{
	if(aLocalRole == EPanRoleU)
		{
		// Check if we can change role.  There must be at least one connection.  Any more than
		// one and we can't change to U role.
		if(ActiveDeviceCount() == 1)
			{
			LOG(_L("RoleState: <CPanAgtStateGnOrNapRole> - ...remote device wants us to be U, accepting"));

			CPanAgtStateBase* nextState = CPanAgtStateRenegotiatingActiveConnectionToURoleAtPeersBehest::NewL(StateMachine());
			StateMachine().SetState(*nextState);
			delete this;
			}
		else
			{
			// Can't change local role.
			User::Leave(KErrInvalidOrUnacceptableRoleCombination);
			}
		}
	else
		{
		// The local role must be the same as our local role.
		// Nothing to do.  Accept request and wait for DeviceActive call.
		CPanAgtStateBase* nextState = CPanAgtStateNegotiatingGnOrNapRole::NewL(StateMachine());
		StateMachine().SetState(*nextState);
		delete this;
		}
	}
	
void CPanAgtStateGnOrNapRole::InitiateOutgoingConnectionL(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles)
	{
	LOG(_L("RoleState: <CPanAgtStateGnOrNapRole> - InitiateOutgoingConnectionL"));

	// There must already be an existing connection (in GN/NAP role).  Set the local role to GN/NAP and the
	// remote to U
	aLocalRole = StateMachine().NapEnabled() ? EPanRoleNap : EPanRoleGn;
	
	// Check the a connection U role is worth trying
	if(aWorthTryingRemoteRoles.IsWorthTryingU())
		{
		aRemoteRole = EPanRoleU;
		}
	else
		{
		User::Leave(KErrInvalidOrUnacceptableRoleCombination);
		}
		
	CPanAgtStateBase* nextState = CPanAgtStateNegotiatingGnOrNapRole::NewL(StateMachine());
	StateMachine().SetState(*nextState);
	delete this;
	}

void CPanAgtStateGnOrNapRole::DeviceActiveL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**

*/
	{
	// A new device has been added to the bridge.
	}
	
void CPanAgtStateGnOrNapRole::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**

*/
	{
	LOG(_L("RoleState: <CPanAgtStateGnOrNapRole> - device disconnected..."));

	DoDeviceDisconnectL(aDevice); // if there are no devices in the array and we're not supporting incoming connection this method will leave
	
	if(StateMachine().Devices().Count())
		{
		LOG1(_L("RoleState: <CPanAgtStateGnOrNapRole> - ...%d device(s) remaining - staying in GN role"), StateMachine().Devices().Count());
		// do nothing - need to remain in GN/NAP role
		}
	else	// no devices left - decide what to do next...
		{
		LOG(_L("RoleState: <CPanAgtStateGnOrNapRole> - ...no devices remaining - choosing next role..."));
		CPanAgtStateBase* nextState = NULL;
		StartListenerL();

		LOG(_L("RoleState: <CPanAgtStateGnOrNapRole> - ...incoming connections not being accepted, going to idle state"));
		nextState = CPanAgtStateIdle::NewL(StateMachine());

		__ASSERT_ALWAYS(nextState, PanicInState(ENoNextStateSet));
		StateMachine().SetState(*nextState);
		delete this;
		return;
		}
	}

void CPanAgtStateGnOrNapRole::StateSendRoleNotification()
	{
	if(StateMachine().NapEnabled())
		{
		StateMachine().SendProgress(EPanAgtNapRole, KErrNone);
		}
	else
		{
		StateMachine().SendProgress(EPanAgtGnRole, KErrNone);
		}
	}
	
//
// CPanAgtStateShutdown
//
CPanAgtStateBase* CPanAgtStateShutdown::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateShutdown* self = new (ELeave) CPanAgtStateShutdown(aStateMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateShutdown::CPanAgtStateShutdown(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateShutdown)
/**

*/
	{	}

CPanAgtStateShutdown::~CPanAgtStateShutdown()
	{
	iShutdownCallback->Cancel();
	delete iShutdownCallback;
	}
void CPanAgtStateShutdown::ConstructL()
	{
	BaseConstructL();
	iShutdownCallback = new (ELeave) CAsyncCallBack(KPanAgtAoPriority);
	TCallBack callbackFunc(ShutdownCallback, this);
	iShutdownCallback->Set(callbackFunc);
	}

TInt CPanAgtStateShutdown::ShutdownCallback(TAny* aThisPtr)
/**
Callback function to do a full shutdown of the Pan Agent
*/
	{
	LOG(_L("RoleState: <shutdown> - shutting down"));

	CPanAgtStateShutdown* self = (CPanAgtStateShutdown*)aThisPtr;
	self->StateMachine().FullShutdownComplete();
	return KErrNone;
	}
	
void CPanAgtStateShutdown::OnEntryL()	
/**
Shut all the connections down
*/
	{
	LOG(_L("RoleState: <shutdown> - entering state"));

	// Start closing things down, and notify apps that we're shutting down
	StateMachine().CloseIncomingConnectionListener();
	StateMachine().SendProgress(EPanAgtDisconnecting, KErrNone);

	// Start disconnecting any attached devices...
	RPointerArray<CPanRemoteDeviceStateMachine>& deviceArray = StateMachine().Devices();
	if(deviceArray.Count()==0)	// ...no devices attached, shutdown straight away
		{
		LOG(_L("RoleState: <shutdown> - no devices connected, use callback to shutdown"));
		
		// Use a Callback function here to allow the connection to process any progress
		// before the shutdown is completed.
		// This is important if a connection is being created from an IAP entry and
		// the connection fails, without allowing the connection to process the progress
		// RConnection::Start will never return.
		iShutdownCallback->CallBack();		
		return;
		}
	else	// ...tell all the attached devices to shut down
		{
		for(TInt i=0; i < deviceArray.Count(); ++i)
			{
			deviceArray[i]->Disconnect();
			}
		}
	}
	
void CPanAgtStateShutdown::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**
Remote device has disconnected
@note *** Must not leave from here ***
*/
	{
	TRAPD(err, DoDeviceDisconnectL(aDevice));
	if(err==KErrAllDevicesDisconnected)	// all devices have disconnected, so we can shutdown now
		{
		LOG(_L("RoleState: <shutdown> - all devices disconnected, shutting down"));
		StateMachine().FullShutdownComplete();
		}
	else if(err)
		{
		LOG1(_L("RoleState: <shutdown> - unknown error %d, shutting down"), err);
		StateMachine().FullShutdownComplete();
		}
	else if(StateMachine().Devices().Count()==0)
		{
		LOG(_L("RoleState: <shutdown> - all devices disconnected but incoming connection listener active, stopping it and shutting down"));
		StateMachine().FullShutdownComplete();
		}
	}

//
// CPanAgtStateReconnect
//
CPanAgtStateBase* CPanAgtStateReconnect::NewL(MPanAgtStateMachineNotify& aStateMachine)
	{
	CPanAgtStateReconnect* self = new (ELeave) CPanAgtStateReconnect(aStateMachine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanAgtStateReconnect::CPanAgtStateReconnect(MPanAgtStateMachineNotify& aStateMachine) :
	CPanAgtStateBase(aStateMachine, EPanAgtStateReconnect)
/**

*/
	{	}

CPanAgtStateReconnect::~CPanAgtStateReconnect()
	{
	iReconnectCompleteCallback->Cancel();
	delete iReconnectCompleteCallback;
	}

void CPanAgtStateReconnect::ConstructL()
	{
	BaseConstructL();
	iReconnectCompleteCallback = new (ELeave) CAsyncCallBack(KPanAgtAoPriority);	
	TCallBack reconnectCompleteCallback(ReconnectCompleteCb, this);
	iReconnectCompleteCallback->Set(reconnectCompleteCallback);
	TCallBack serviceStartedCallback(ServiceStartedCb, this);
	iServiceStartedCallback->Set(serviceStartedCallback);
	}

void CPanAgtStateReconnect::OnEntryL()	
	{
	LOG(_L("RoleState: <reconnect> - entering state"));

	StateMachine().CloseIncomingConnectionListener();

	// Start disconnecting any attached devices...
	RPointerArray<CPanRemoteDeviceStateMachine>& deviceArray = StateMachine().Devices();
	if(deviceArray.Count()!=0)	// ...no devices attached, shutdown straight away
		{
		for(TInt i=0; i < deviceArray.Count(); ++i)
			{
			deviceArray[i]->Disconnect();
			}
		}
	iReconnectCompleteCallback->CallBack();
	}

void CPanAgtStateReconnect::Connect()
	{
	iServiceStartedCallback->CallBack();
	}

void CPanAgtStateReconnect::CancelReconnect()
	{
	iReconnectCompleteCallback->Cancel();
	}

void CPanAgtStateReconnect::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& aDevice)
/**
Remote device has disconnected
*/
	{
	DoDeviceDisconnectL(aDevice);
	}

void CPanAgtStateReconnect::ServiceStarted()
/**
@note Don't notify Nifman with ServiceStarted() because that will try to load the packet driver
again which will cause a panic.
*/
	{
	LOG(_L("CPanAgtStateBase: Service started"));

	StateMachine().SendProgress(EPanAgtInitialising, KErrNone);
	iConnectCompleteCallback->CallBack();
	}

TInt CPanAgtStateReconnect::ServiceStartedCb(TAny* aThisPtr)
/**
Connection callback function 
*/
	{
	__ASSERT_DEBUG(aThisPtr, PanAgentPanic(ENullTAnyPointer));

	CPanAgtStateReconnect* self = static_cast<CPanAgtStateReconnect*>(aThisPtr);
	self->ServiceStarted();
	return KErrNone;
	}
	
TInt CPanAgtStateReconnect::ReconnectCompleteCb(TAny* aThisPtr)
/**
Reconnect callback function
*/
	{
	__ASSERT_DEBUG(aThisPtr, PanAgentPanic(ENullTAnyPointer));

	CPanAgtStateReconnect* self = static_cast<CPanAgtStateReconnect*>(aThisPtr);
	self->StateMachine().ReconnectComplete(KErrNone);
	return KErrNone;
	}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CPanAgtStatePaused* CPanAgtStatePaused::NewL(MPanAgtStateMachineNotify& aStateMachine, CPanAgtStateBase& aNextState)
    {
    CPanAgtStatePaused* self = new(ELeave) CPanAgtStatePaused(aStateMachine, aNextState);
    CleanupStack::PushL(self);
	self->BaseConstructL();
    CleanupStack::Pop();
    return self;
    }

CPanAgtStatePaused::CPanAgtStatePaused(MPanAgtStateMachineNotify& aStateMachine, CPanAgtStateBase& aNextState)
    : CPanAgtStateBase(aStateMachine, EPanAgtStatePaused), iNextState(&aNextState)
    {
    }
    
CPanAgtStatePaused::~CPanAgtStatePaused()
    {
    delete iNextState;
    }

void CPanAgtStatePaused::OnEntryL()
    {
    // Does nothing
    }
    
void CPanAgtStatePaused::TransitionToNextState()
    {
    StateMachine().SetState(*iNextState);
    
    // We no longer own the next state
    iNextState = NULL;
    delete this;
    }
    
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
