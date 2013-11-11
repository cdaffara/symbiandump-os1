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

#ifndef PANAGTUTILS_H
#define PANAGTUTILS_H

#include <e32std.h>
#include <e32base.h>

/**
@file
@note Global panic function and async event helper class for role state machine
The async event helper class is used because the main agent class cannot be derived from CActive, so the helper class is just a small proxy to allow the role state machine to handle async events
*/

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

_LIT(KPanAgentName, "PanAgent");
_LIT(KPanAgentPanic, "PanAgentPanic");

enum TPanAgentPanic
/**
PAN agent panic codes
@internalComponent
*/
	{
	// General panics
	ENullNifmanNotifyPointer = 0,								//< Attempt to use iNotify pointer to nifman when it was null
	ENullCommdbHandle = 1,										//< Attempt to access commdb when the agent has not got a handle yet
	ENullBnepConnectionManager = 2,								//< Attempt to use the BNEP connection manager before the handle has been fetched from the BNEP packet driver
	EPanAgentDeletedWithDevicesStillAttached = 3,				//< Attempt to delete the pan agent when there are still devices in the device array
	ENullDeviceSelector = 4,									//< Attempt to use the device selector when is doesn't exist
	ENullTAnyPointer = 5,										//< Someone's tried to perform a callback with a NULL pointer instead of the this pointer
	EOldFormatPanRoleBeingUsedInCommdb = 6,						//< An old format commdb is being used, which contains a PAN role of 0xffff. This was removed during development (there was no need for EPanRoleUnknown and EPanRoleNone, so they were folded into one). This panic is here purely to stop people using those old commdb databases by mistake.
	ESomeoneLeftWithoutGivingAReason = 7,						//< A state has left, but no-one recorded an error code to indicate what the problem was!
	EAttemptToDisconnectBeforeConnectComplete = 8,				//< Attempt to use the wrong error path - before ConnectComplete() has been called on nifman, all errors should be signalled via there; only once connected should we try to disconnect
	EAsyncOperationOnOutgoingConnectionStarterWhenItsNotPerformingAnAsyncRequest = 9, //< Attempt to call RunL() or DoCancel() on the outgoing connection starter when it's not doing anything that requires an async operation - this suggests it has got into an incorrect state
	EDelayedOpenHelperActiveWhenBothListeningAndAcceptingSocketsAreOpen = 10,
	// PAN role state machine related panics
	ENoState = 11,												//< Attempted call to state when there isn't one
	EPanAgentRoleStateMachineSelectedUnknownAsLocalRole = 12,	//< The PAN agent role state machine selected an illegal role as the local role
	EPanAgentRoleStateMachineInvalidLocalRole = 13,				//< The PAN agent role state machine selected an invalid local role.
	EDeviceInNegotiatingModeOnEntryToUState = 14,				//< Suggests that the negotiating state forgot to update the state of the device in the array when negotiation completed
	EDeviceInNegotiatingModeOnEntryToGnOrNapState = 15,				//< Suggests that the negotiating state forgot to update the state of the device in the array when negotiation completed
	EDeviceAttemptingToNegotiateThatIsNotInArray = 16,			//< Attempt to negotiate by a device that is not in the array
	EIncomingConnectionListenerStillActiveAtShutdown = 17,		//< No-one's cancelled the incoming connection listener before shutting down
	EActiveDevicesInArrayInInitialisingState = 18,				//< There are active devices in the array when the role state machine is in the initialising state (so either there was an incorrect transition to this state in the role state machine, or someone's put entries in the device array in the wrong state)
	ENegotiatingDevicesInArrayInInitialisingState = 19,			//< There are negotiating devices in the array when the role state machine is in the initialising state (so either there was an incorrect transition to this state in the role state machine, or someone's put entries in the device array in the wrong state)
	EActiveDevicesInArrayInNegotiatingPanUState = 20,			//< There are active devices in the array when we are negotiating PAN-U role (in which case we shouldn't be negotiating PAN-U role)
	EActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtOurBehest = 21, //< There is an active device in the array, and since we negotiating with a device to go to GN/NAP state, there shouldn't be (we're not in GN/NAP state yet!)
	ENoActiveDeviceInArrayInRenegActConnToURoleAtPeersBehest = 22, //< There is NOT an active device in the array
	ENoActiveDevicesInArrayInRenegActConnToGnOrNapRoleAtPeersBehest = 23, //< There is NOT an active device in the array
	EMoreThanOneDeviceInNegotiatingModeInArray = 24,				//< There is more than one negotiating device in the array (which there shouldn't be, as negotiation should be serialised)
	EUnexpectedDeviceInNegotiatingModeInArray = 25,				//< A local device is in the negotiating state when it shouldn't be.
	EDeletingRemoteDeviceWithoutDisconnecting = 26,				//< The remote device object has been deleted without disconnecting the remote device first (suggests an incorrect state transition in remote device state machine)
	ETooManyDevicesAttachedInURole = 27,							//< Attempt to enter a PAN-U role with more than one device attached
	EUnexpectedDeviceActiveReceivedByState = 28,					//< DeviceActive() received by state that does not allow active devices
	EUnexpectedDeviceRoleChangeFailedReceivedByState = 29,		//< DeviceRoleChangeFailed() received by state in which devices should not be changing roles 
	EUnexpectedDeviceDisconnectedReceivedByState = 30,			//< DeviceDisconnected() received by state that does not allow devices to disconnect
	EUnexpectedIncomingConnectionFromPeerReceivedByState = 31,	//< Incoming connection from peer received by state in which devices should not be receiving incoming connections
	EUnexpectedRoleChangeRequestFromPeerReceivedByState = 32,	//< Role change request from peer received by state in which devices should not be receiving incoming role changes from the peer device
	EUnexpectedInitiateOutgoingConnectionReceivedByState = 33,	//< Initiate outgoing connectionreceived by state in which devices should not be initiating an outgoing connection
	EUnexpectedPerformLocalRoleChangeRequestReceivedByState = 34, //< Local r	ole change request received by state in which devices should not be perfroming a local role change
	EDeviceActiveCalledByDeviceNotInActiveState = 35,			//< A device has sent a DeviceActive() message, but it is not in the Active state
	EDeviceDisconnectedThatWasNotInArray = 36,
	ETooManyDevicesAttachedWhenRenegotiatingActiveConnectionForGnOrNapUpgrade = 37, //< The renegotiating active connection for GN/NAP upgrade state can only be used from U role
	ENoActiveDeviceInArray = 38,									//< We're trying to find an active device and there isn't one in the array
	EShutdownStateNotLastState = 39,								//< The PAN agent was deleted without the shutdown state being the last state used.
	ENoActiveDeviceAfterNegotiationCompleted = 40,				//< A DeviceActive() was received, but there is no active device in the array
	EMoreThanOneActiveDeviceWhenTryingToRenegotiateUToGnOrNapRoleChange = 41, //< We're attempt to upgrade from U to GN/NAP, but there's more than one active device in the array (which is wrong if we've just come out of U role)
	ETryingToLockBasebandRolesWhenDevicesAreNegotiating = 42,	//< Don't want to do this - we should only try locking roles when we're in a non-negotiating state
	EFixedLocalRoleUForListeningIap = 43,							//< A fixed local role is set for a listening IAP.
	EActiveDeviceThatIsNotInArray = 44,
	EDeviceNotActive = 45,
	EInvalidLocalRoleFromRoleChangeRequestFromPeer = 46,			//< Invalid role provide to RoleChangeRequestFromPeer method
	EInvalidRoleForIncomingConnectionFromPeerL = 47,				//< Invalid role provide to IncomingConnectionFromPeerL method
	// PAN remote device state machine related panics
	ENoRemDevState = 48,											//< Attempted call to state when there isn't one
	ESendingUnknownRoleInRoleRequest = 49,						//< Attempt to send EPanRoleUnknown in a role request - this is an absolute no-no
	ECallToBnepChannelBeforeChannelSetup = 50,					//< Attempt to use BNEP channel to remote device before it has been created
	EUnexpectedAsyncEventReceivedByState = 51,					//< A state has received an asynchronous event (from someone signalling the state machine's TRequestStatus), but wasn't expecting to (ie. there's something wrong with the sequence of states)
	EInvalidReadyForRoleRequestReceived = 52,					//< The remote device object has been incorrectly signalled by the main role state machine (the remote device is not in a state where it can performing role negotiation)
	EInvalidBnepRoleResponseReceived = 53,						//< The remote device object has been incorrectly signalled by BNEP indicating the remote device has responded to a role request (this occurs in states where the remote device has not been connected yet)
	EInvalidBnepRoleRequestReceived = 54,						//< The remote device object has been incorrectly signalled by BNEP indicating the remote device has sent us a role request (this occurs in states where the remote device has not been connected yet)
	EInvalidRemoteDeviceDisconnectReceived = 55,					//< The remote device object has been incorrectly signalled by BNEP indicating the remote device has disconnected (this occurs in states where the remote device has not been connected yet)
	ENoStateOnExit = 56,											//< The remote device object is being deleted without being in any state (this suggests a serious problem - why was it created in the first place?)
	EPanAgentRoleStateMachineReturnedUnexpectedErrorCode = 57,	//< The RoleRequest() returned an unexpected error code which we could not handle
	EChannelControllerAlreadyExists = 58,						//< The remote device object is trying to create a BNEP channel when it already has one
	ETryingToDecideWhetherToBecomePiconetMasterBasedOnInvalidRole = 59, //< like it says, the local role was not U, GN, or NAP
	ENullRemoteDeviceAddress = 60,								//< The address of the remote device is not set
	EDoubleShutdownAttemptOnDevice = 61,							//< Two attempts have been made to use the preallocated shutdown state (perhaps the shutdown state itself left?)
	ERemoteDeviceDidNotUsePreallocatedShutdownState = 62,		//< Attempt to shutdown by using something other than the state machine's internally held shutdown state (someone's new'ed up a shutdown state themselves)
	ENoNextStateSet = 63,										//< No state was chosen as the next state
	ERoleStateMachineRequestedWeRenegotiateRolesThenReturnedLocked = 64, //< If the role state machine triggers renegotiation on a device it should ensure it is ready to accept a role request (if it isn't, then something's gone very wrong)
	ERoleStateMachineCalledReadyForRoleRequestThenReturnedLocked = 65, //< The role state machine told us it was unlocked, then returned locked!  Must be bug in the role state machine
	EStillActiveOnExit = 66,										//< There's still a request outstanding when we're being deleted!
	EInvalidResultCodeFromNapUplinkAuthorisationNotifier = 67,		//< Invalid result code received from Local Role selection notifier
	// Local SDP helper class panics
	EAttemptToRegisterIllegalPanRoleInLocalSdpDatabase = 68, 	//< Someone's trying to register a local role other than PAN U, GN, or NAP
	EPanRoleOutOfBounds = 69,									//< The PAN role were were sent is invalid, and therefore cannot be used as an index to the SDP registration array
	// CPanAgtRemDevSelector panics
	EPanAgtRemDevSelectorNoDevicesToConnect = 70,				//The dev selector is asked to make a connection while there is no devices in the list
	
	EPanAgtUnexpectedMessage = 71,                               // An sync message was received from somewhere that was not expected
	EPanAgtUnexpectedStateChange = 72,                           // A response from BNEP was received but the state machine was not in a Paused state (meaning something else unexpectedly changed it while we waited)
	EPanAgtMissingBinding = 73,                                  // There is probably a binding missing between ESock_IP and ESock_BT (also worth checking that there is one between ESock_DIP and ESock_BT)

	EInvalidRemoteDeviceState = 74,

	//RNDIS&BT P&S panics
	EIPBearerCoexistancePropertyIsNotDefined = 75,                // The property is not defined when we require it, maybe it's deleted by mistakes.
	EBTPanIsAlreadyActive = 76,								     // BT PAN is active in a improper situation.
	ERndisIsActiveWhenBTPanTriesToBeActive = 77,                  // Rndis is allready active when BT Pan tries to be active
	EInvalidIPBearerCoexistanceProperty = 78,                     // Facing an invalid IP bearer property
	EClearKeyWhenNoneIsActive = 79,		                         // BT PAN is going to clear the property, but non-service is active
	EIPBearerCoexistancePropertyCanNotBeDefined = 80,             // The property define returns error other than KErrAlreadyExists
	EIPBearerCoexistancePropertySetFailed = 81,                   // Setting Property failed 
	EIPBearerCoexistancePropertyGetFailed = 82,	                 // Getting Property failed
	EPropertyIsNoneIsActiveWhenBTPanIsActive = 83,                // Property is none active when BT Pan is active
	EImproperSituationToSetPropertyToBTPanIsActive = 84,          // Set property to BT PAN is active in an improper situation
	EBTPanInitiateDirectTransitionToRndis = 85,                   // BT PAN Initiate a direct transition to Rndis
	ECreatePanCoexistenceConnectionControllerFailed = 86,         // Contruct PAN Coexistence connection controller failed during the contruction time
	EIPBearerCoexistancePropertyShouldnotBePanIsActive = 87,      // The P&S key should not be BT PAN active
	ENoPanConnectionExistsWhileInPanIsActiveCoexistencePandSState = 88, //P&S key is BT PAN active when there is no BT PAN connection existing
	};

/**
Panics raised by state machine default behaviour are multiplied by this
factor.  The less signifigant digits are then available to carry a state
index, allowing more information when fixing defects.

For example, a unexpected device active passed to the PAN local state machine may 
raise panic 'PanAgentPanic 321008' if it occurs in the 'Negotiating Gn Role' state.
*/
const TInt KStatePanicMultiplier = 10000;

/**
A delimiter to prevent a panic code of 0 creating ambigious panics.
Without this panic code 0 in state 8 and panic code 8 (not in the state 
machine) would be identical.
*/
const TInt KStatePanicDelimiter = 1000; // '1' is the delimiter

void PanAgentPanic(TPanAgentPanic aPanicCode);
void PanAgentPanic(TPanAgentPanic aPanicCode, TInt aState);
	
class MPanAgtTimerCallback
/**
Callback from timer helper class
@internalComponent
*/
	{
public:
	virtual void TimerComplete() = 0;
	virtual void TimerError(TInt aError) = 0;
	
protected:
	virtual ~MPanAgtTimerCallback() {};
	};
	
NONSHARABLE_CLASS(CPanAgtTimerHelper) : public CActive
/**
A helper class for a state that is already using its TRS-type async event for something else
@internalComponent
*/
	{
public:
	static CPanAgtTimerHelper* NewL(MPanAgtTimerCallback& aCallback);
	void SetTimer(TTimeIntervalMicroSeconds32 aInterval);
	
	// CActive-derived methods
	virtual void RunL();
	virtual void DoCancel();

private:
	CPanAgtTimerHelper(MPanAgtTimerCallback& aCallback);
	void ConstructL();

private:
	RTimer iTimer;
	MPanAgtTimerCallback& iCallback;
	};

}	// PanAgent
#endif // PANAGTUTILS_H
