// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @note PAN agent remote device class implementation, and remote device state base class implementation
*/

#include <bluetooth/logger.h>
#include "panagtlog.h"
#include "panagtremdev.h"
#include "panagtremdevstates.h"

using namespace PanAgent;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <elements/nm_signals.h>
#include "panmessages.h"

using namespace ESock;
using namespace Messages;

#ifdef ESOCK_EXTLOG_ACTIVE
_LIT8(KPanAgtSubTag, "panagt");
#endif

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

CPanRemoteDeviceStateMachine* CPanRemoteDeviceStateMachine::NewL(RInternalSocket& aConnectedSocket, MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase)
/**
Create a new bnep channel in the nif using an already connected socket.
@note This is for incoming connections only.
@param aConnectedSocket A preconnected L2CAP socket ready to hand off to BNEP
@param aParent The class to be notified when events occur
@param aDatabase An handle to the commdb facade to allow us to read records from it
*/
	{
	CPanRemoteDeviceStateMachine* self = new(ELeave) CPanRemoteDeviceStateMachine(aParent, aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL(aConnectedSocket);
	CleanupStack::Pop(self);
	return(self);
	}

CPanRemoteDeviceStateMachine* CPanRemoteDeviceStateMachine::NewL(const TBTDevAddr& aRemDevAddr, MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase)
/**
Create a new BNEP channel in the nif by connecting a socket and passing it across.
@note This is for outgoing connections only.
@param aRemDevAddr A preconnected L2CAP socket ready to hand off to BNEP
@param aParent The class to be notified when events occur
@param aDatabase An handle to the commdb facade to allow us to read records from it
*/
	{
	CPanRemoteDeviceStateMachine* self = new(ELeave) CPanRemoteDeviceStateMachine(aParent, aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL(aRemDevAddr);
	CleanupStack::Pop(self);
	return(self);
	}

CPanRemoteDeviceStateMachine::CPanRemoteDeviceStateMachine(MRemoteDeviceNotify& aParent, CCommsDbAccess& aDatabase) :
	CActive(KPanAgtAoPriority), iParent(aParent), iDatabase(aDatabase)
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NM_LOG_NODE_CREATE(KPanAgtSubTag, CPanRemoteDeviceStateMachine);
#endif
	CONNECT_LOGGER
	CActiveScheduler::Add(this);
	}

void CPanRemoteDeviceStateMachine::ConstructL(const TBTDevAddr& aRemDevAddr)
/**
Connect a new socket and create a BNEP channel to use it.
@note This happens for outgoing connections only.
*/
	{
	iRemSockAddr.SetBTAddr(aRemDevAddr);
	iRemSockAddr.SetPort(KBnepPsm);
	User::LeaveIfError(iSockServ.Connect());
	
#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	iRemSockAddr.BTAddr().GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG2(_L("RemDev[%x]: connected to %S"), this, &tempDevAddrBuf);
#endif

	// preallocate the shutdown state
	iShutdownState = new(ELeave) CPanRemDevStateShutdown(*this);

	LOG(_L("RemDevStateMachine - Deciding whether to perform SDP query"));
	TBool sdpQueriesDisabledStatus;
	Database().GetBoolL(TPtrC(PAN_SERVICE_EXTENSIONS), TPtrC(PAN_DISABLE_SDP_QUERY), sdpQueriesDisabledStatus);

	SdpQueriesDisabled(sdpQueriesDisabledStatus);

	// if we're allowed to use SDP, then perform the SDP query
	CPanRemDevStateBase* nextState;
	if(!IsSdpQueriesDisabled())
		{
		// open an SDP query subsession	
		LOG(_L("RemDevStateMachine - Connected to remote SDP querier"));
		User::LeaveIfError(RemoteSdpQuerier().Open());

		nextState = new(ELeave) CPanRemDevStatePerformingSdpQuery(*this);
		}
	else	// SDP queries are inhibited by setting in commdb
		{	
		LOG(_L("RemDevStateMachine  - SDP query inhibited by commdb settings"));
		nextState = new(ELeave) CPanRemDevStateConnectingSocket(*this);
		}
	SetState(*nextState);
	}

void CPanRemoteDeviceStateMachine::ConstructL(RInternalSocket& aConnectedSocket)
/**
Take a preconnected socket and create a BNEP channel to use it.
@note This happens for incoming connections only.
*/
	{
	User::LeaveIfError(iSocket.Transfer(aConnectedSocket)); // take ownership of the socket, so that the initial state can perform actions on it

	// make a note of the remote device address
	iSocket.RemoteName(iRemSockAddr);
	User::LeaveIfError(iSockServ.Connect());

#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	iRemSockAddr.BTAddr().GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG2(_L("RemDev[%x]: connected to %S"), this, &tempDevAddrBuf);
#endif

	// preallocate the shutdown state
	iShutdownState = new(ELeave) CPanRemDevStateShutdown(*this);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    // Create the states first so if we Leave no message is sent to BNEP
	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingRoleNegotiationForIncomingConnection(*this);
    CleanupStack::PushL(nextState);
    CPanRemDevStatePaused* pausedState = new(ELeave)CPanRemDevStatePaused(*this, *nextState);
    CleanupStack::Pop();
	SetState(*pausedState);
	
    CreateNewBnepConnection(iSocket, TPanMessage::EActivityCreateChannelControllerForIncoming);
	OpenPhysicalLinkAdapterL();

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	// create the connection
	CreateNewBnepConnectionL(iSocket);
	OpenPhysicalLinkAdapterL();

	CPanRemDevStateBase* nextState = new(ELeave) CPanRemDevStatePerformingRoleNegotiationForIncomingConnection(*this);
	SetState(*nextState);
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	}

CPanRemoteDeviceStateMachine::~CPanRemoteDeviceStateMachine()
/**
Delete the last state on the way out, and tidy up various other bits and pieces
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	__ASSERT_DEBUG(iBnepChannelController == TNodeId::NullId(), PanAgentPanic(EDeletingRemoteDeviceWithoutDisconnecting));
#else
	__ASSERT_DEBUG(!iBnepChannelController, PanAgentPanic(EDeletingRemoteDeviceWithoutDisconnecting));
#endif

	__ASSERT_DEBUG(iState, PanAgentPanic(ENoStateOnExit));
	__ASSERT_DEBUG(!IsActive(), PanAgentPanic(EStillActiveOnExit));
	
	Cancel(); // in release builds, cancel the outstanding request
	
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if(iBnepChannelController)
		iBnepChannelController->Close();
#endif
	
	delete iState;
	iState = NULL;
	
	__ASSERT_DEBUG(!iShutdownState, PanAgentPanic(ERemoteDeviceDidNotUsePreallocatedShutdownState));
	// everyone should be shutting down by leaving from the appropriate method and letting the 
	// state machine clean up/shut down the connection.  If this pointer is not NULL, it suggests
	// that someone's created the shutdown state directly (which can fail in OOM situations)
	delete iShutdownState; // delete the shutdown state if it exists
	iSocket.Close(); // shouldn't be open, but just in case
	iPhysicalLinkAdapter.Close();
	iRemoteSdpQuerier.Close();
	iSockServ.Close();

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NM_LOG_NODE_DESTROY(KPanAgtSubTag, CPanRemoteDeviceStateMachine);
#endif
    CLOSE_LOGGER
	}
	
		
//
// CActive methods
//

void CPanRemoteDeviceStateMachine::RunL()
/**
Called when the new socket is connected
@note Only used when creating a new socket for an outgoing connection
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));
	
	TRAPD(err, iState->AsyncEventCompleteL());
	if(err)
		{
		Shutdown(err);
		}
	}

void CPanRemoteDeviceStateMachine::DoCancel()
/**
Cancel the connection
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));
	
	TRAPD(err, iState->AsyncEventCancelL());
	if(err)
		{
		Shutdown(err);
		}
	}

//
// Other state machine downcall methods
//

void CPanRemoteDeviceStateMachine::ReadyForRoleRequest()
/**
Pass the request from the local device to the appropriate control channel.
Called from the state machine when it's ready to negotiate roles for this connection
@note This can be called when the connection is active, and indicates that the state machine wants to renegotiate roles
@note Clients must set a flag and initiate a callback to start the RoleRequest() process to prevent call stacks from getting too large
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));

	TRAPD(err, iState->ReadyForRoleRequestL());
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}
	
void CPanRemoteDeviceStateMachine::Disconnect()
/**
Locally initiated disconnect
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));

	TRAPD(err, iState->ShutdownL());	
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}

TInt CPanRemoteDeviceStateMachine::DisallowRoleSwitch()
/**
Change the link policy to prevent other devices performing master/slave switches
@note This is intended to lock us in the master role
*/
	{
	TInt err = iPhysicalLinkAdapter.PreventRoleSwitch();

#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	iRemSockAddr.BTAddr().GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG2(_L("RemDev[%S]: ...prevent role change returned %d"), &tempDevAddrBuf, err);
#endif

	return err;
	}
	
TInt CPanRemoteDeviceStateMachine::AllowRoleSwitch()
/**
Change the link policy to allow other devices to perform master/slave switches
*/
	{
	TInt err = iPhysicalLinkAdapter.AllowRoleSwitch();

#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> tempDevAddrBuf;
	iRemSockAddr.BTAddr().GetReadable(tempDevAddrBuf, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG2(_L("RemDev[%S]: ...allow role change returned %d"), &tempDevAddrBuf, err);
#endif

	return err;
	}

TBTDevAddr CPanRemoteDeviceStateMachine::DevAddr() const
/**
Return the device address of the remote device
*/
	{
	return(iRemSockAddr.BTAddr());
	}

//
// MPanConnectionNotify methods
//

void CPanRemoteDeviceStateMachine::BnepRoleRequestFromRemoteDevice(const TUUID& aRequestedLocalRole, const TUUID& aRequestedRemoteRole)
/**
Request from remote device for given BNEP roles
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));
	
	TRAPD(err, iState->BnepRoleRequestFromRemoteDeviceL(aRequestedLocalRole, aRequestedRemoteRole));
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}
	
void CPanRemoteDeviceStateMachine::BnepRoleResponseFromRemoteDevice(TBnepSetupConnectionResponseMessage aRoleResponseCode)
/**
Response from remote device to our request for BNEP roles
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));
	
	TRAPD(err, iState->BnepRoleResponseFromRemoteDeviceL(aRoleResponseCode));
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}

void CPanRemoteDeviceStateMachine::SetRetryConnect()
	{
	iParent.SetRetryConnect(*this);
	}
	
void CPanRemoteDeviceStateMachine::RemoteDeviceDisconnect(TInt aError)
/**
The remote device has disconnected
*/
	{
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoRemDevState));
	
	TRAPD(err, iState->RemoteDeviceDisconnectL(aError));
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}

TInt CPanRemoteDeviceStateMachine::IncrementConnectionRetryAttempts()
	{
	return iConnectionRetryAttempts++;
	}

void CPanRemoteDeviceStateMachine::ResetConnectionRetryAttempts()
	{
	iConnectionRetryAttempts = 0;
	}

void CPanRemoteDeviceStateMachine::ResetRetryParameters()
	{
	iRemoteWorthTryingRoles.ResetWorthTryingRoles();
	}

TBool CPanRemoteDeviceStateMachine::WorthTrying()
	{
	LOG(_L("CPanRemoteDeviceStateMachine::WorthTrying"));
	TBool retval = EFalse; 	// assume its not worth retrying until we prove otherwise
	
	if(iParent.WorthTrying(iRemoteWorthTryingRoles))
		{
		if (RemoteWorthTryingRolesList().IsWorthTryingU())
			{
			retval = ETrue;
			}
		else if (RemoteWorthTryingRolesList().IsWorthTryingGn())
			{
			retval = ETrue;
			}
		else if (RemoteWorthTryingRolesList().IsWorthTryingNap())
			{
			retval = ETrue;
			}
		}
	return(retval);
	}

TRemoteDeviceState CPanRemoteDeviceStateMachine::GetState() const
	{
	TRemoteDeviceState state = EIdle;
	if(iState)
		{
		state = iState->GetState();
		}
	return state;
	}

TBool CPanRemoteDeviceStateMachine::NewConnectedDeviceProgressSent() const
	{
	return iNewConnectedDeviceProgressSent;
	}
	
void CPanRemoteDeviceStateMachine::SetNewConnectedDeviceProgressSent(TBool aNewConnectedDeviceProgressSent)
	{
	iNewConnectedDeviceProgressSent = aNewConnectedDeviceProgressSent;
	}

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
MPanLinkControlBase& CPanRemoteDeviceStateMachine::BnepConnectionController() 
	{
	return iParent.BnepConnectionController();
	}
#endif



#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
*/
void CPanRemoteDeviceStateMachine::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& /* aRecipient */, Messages::TSignatureBase& aCFMessage)
    {
	if (aCFMessage.MessageId().Realm() == TPanMessage::ERealmId)
    	{
        switch (aCFMessage.MessageId().MessageId())
            {
            case TPanMessage::TChannelControllerCreated::EId:
                {
                const TNodeCtxId& ctxId = address_cast<TNodeCtxId> (aSender);
                TUint activityId = ctxId.NodeCtx();
                TPanMessage::TChannelControllerCreated& msg = message_cast<TPanMessage::TChannelControllerCreated>(aCFMessage);
                if (activityId == TPanMessage::EActivityCreateChannelControllerForIncoming) 
                    {
                    // We now know that the socket has been transfered, or an error has occured
                    // and that either way it is now safe for the incoming connection listener
                    // to continue accepting connections
                    iParent.RestartIncomingConnectionListener(msg.iValue);
                    }

                if (msg.iValue == KErrNone) 
                    {
                    // Channel controller created - transition into whatever next
                    // state is waiting
                    iBnepChannelController = aSender;

                    // We would expect that iState should always be the paused state when
                    // we get here. We assert it in debug builds in case something we
                    // haven't accounted for changes it unexpectedly
                    __ASSERT_DEBUG(iState->iStateNumber == CPanRemDevStateBase::EPanRemDevStatePaused, PanAgentPanic(EPanAgtUnexpectedStateChange));
                    if (iState->iStateNumber == CPanRemDevStateBase::EPanRemDevStatePaused)
                        {
                        // Calling AsyncEventCompleteL() on the paused state will make it
                        // transition to the real state, start its OnEntryL() processing 
                        TRAPD(err, iState->AsyncEventCompleteL());
                        if (err != KErrNone)
                            {
                            Shutdown(err);
                            }
                        }
                    }
                else
                    {
                    // Error handling for the TCreateChannelController
                    if (activityId == TPanMessage::EActivityCreateChannelControllerForIncoming)
                        {
                        // For outgoing (initial connect or retry connect) the handling should
                        // be a call to Shutdown(msg.iValue);
                        Shutdown(msg.iValue);                        
                        }
                    else if (activityId != TPanMessage::EActivityCreateChannelControllerForIncoming)
                        {
                        __ASSERT_DEBUG(EFalse, PanAgentPanic(EPanAgtUnexpectedMessage));
                        }
                    }
                }
                break;

            case TPanMessage::TRoleRequestFromRemoteDevice::EId:
                {
                TPanMessage::TRoleRequestFromRemoteDevice& msg = message_cast<TPanMessage::TRoleRequestFromRemoteDevice>(aCFMessage);
                BnepRoleRequestFromRemoteDevice(msg.iRequestedLocalRole, msg.iRequestedRemoteRole);
                }
                break;
            
            case TPanMessage::TRoleResponseFromRemoteDevice::EId:
                {
                TPanMessage::TRoleResponseFromRemoteDevice& msg = message_cast<TPanMessage::TRoleResponseFromRemoteDevice>(aCFMessage);
                BnepRoleResponseFromRemoteDevice(msg.iSetupResponse);
                }
                break;
            
            case TPanMessage::TRemoteDeviceDisconnect::EId:
                {
                TPanMessage::TRemoteDeviceDisconnect& msg = message_cast<TPanMessage::TRemoteDeviceDisconnect>(aCFMessage);
                RemoteDeviceDisconnect(msg.iValue);
                }
                break;
                
            default:
                __ASSERT_DEBUG(EFalse, PanAgentPanic(EPanAgtUnexpectedMessage));
                break;
            }
    	}
    else
        {
        __ASSERT_DEBUG(EFalse, PanAgentPanic(EPanAgtUnexpectedMessage));
        }
    
    // Absorb messages
    aCFMessage.ClearMessageId();
    }
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


//
// MPanAgtRemDevStateMachineNotify methods
//

//
// getter methods
//
TBluetoothPanRole& CPanRemoteDeviceStateMachine::LocalRole()
/**
Get a handle to the local role
*/
	{
	return(iLocalRole);
	}
	
TBluetoothPanRole& CPanRemoteDeviceStateMachine::RemoteRole()
/**
Get a handle to the remote role
*/
	{
	return(iRemoteRole);
	}

RPanRemoteSdpQuerier& CPanRemoteDeviceStateMachine::RemoteSdpQuerier()
/**
Get handle to PAN remote SDP querier
*/
	{
	return(iRemoteSdpQuerier);
	}
	

TPanDeviceRolesList& CPanRemoteDeviceStateMachine::RemoteRolesList()
/**
Return a handle to the remote device roles list
*/
	{
	return(iRemotePanRolesFromSdp);
	}

TPanDeviceWorthTryingRolesList& CPanRemoteDeviceStateMachine::RemoteWorthTryingRolesList()
/**
Return a handle to the remote device roles list
*/
	{
	return(iRemoteWorthTryingRoles);
	}

TBTSockAddr& CPanRemoteDeviceStateMachine::RemSockAddr()
/**
Return the remote device address
*/
	{
	return(iRemSockAddr);
	}
	
RInternalSocket& CPanRemoteDeviceStateMachine::Socket()
/**
Get a handle to the socket
*/
	{
	return(iSocket);
	}

CCommsDbAccess& CPanRemoteDeviceStateMachine::Database()
/**
Get a handle to commdb
*/
	{
	return(iDatabase);
	}
	
TRequestStatus& CPanRemoteDeviceStateMachine::Status()
/**
Return this objects TRequestStatus
*/
	{
	return(iStatus);
	}
	
TBool CPanRemoteDeviceStateMachine::IsSdpQueriesDisabled()
/**
Are SDP queries disabled by settings in commdb?
*/
	{
	return(iSdpQueriesDisabled);
	}
void CPanRemoteDeviceStateMachine::SdpQueriesDisabled(TBool aSdpQueriesStatus)
	{
	iSdpQueriesDisabled = aSdpQueriesStatus;
	}
	
TInt CPanRemoteDeviceStateMachine::Error() const
/**
Return the last error to occur
*/
	{
	return(iError);
	}

TBool CPanRemoteDeviceStateMachine::HasBnepChannel() const
/**
Have we got to a stage where we have a BNEP channel open?
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	return(iBnepChannelController != TNodeId::NullId() ? ETrue : EFalse);
#else
	return(iBnepChannelController ? ETrue : EFalse);
#endif
	}

//
// do'er methods
//
void CPanRemoteDeviceStateMachine::SetActive()
/**
Set this object active
*/
	{
	CActive::SetActive();
	}
	
void CPanRemoteDeviceStateMachine::Cancel()
/**
Cancel the outstanding request
*/
	{
	CActive::Cancel();
	}

void CPanRemoteDeviceStateMachine::DeviceActive()
/**
Inform the PAN agent role state machine that we are active
*/
	{
	iParent.DeviceActive(*this);
	}
	
void CPanRemoteDeviceStateMachine::RoleChangeFailed()
/**
Inform the PAN agent role state machine that the role upgrade failed
*/
	{
	iParent.DeviceRoleChangeFailed(*this);
	}

void CPanRemoteDeviceStateMachine::ShutdownComplete()
/**
Inform the PAN agent role state machine that we have shut down
*/
	{
	iParent.DeviceDisconnected(*this);
	}

TInt CPanRemoteDeviceStateMachine::InitiateOutgoingConnection(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles)
	{
	return iParent.InitiateOutgoingConnection(aLocalRole, aRemoteRole, aWorthTryingRemoteRoles);
	}

TInt CPanRemoteDeviceStateMachine::PerformLocalRoleChangeRequest(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole)
	{
	return iParent.PerformLocalRoleChangeRequest(aLocalRole, aRemoteRole);
	}

TInt CPanRemoteDeviceStateMachine::RoleChangeRequestFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole)
	{
	return iParent.RoleChangeRequestFromPeer(aLocalRole, aRemoteRole);
	}

TInt CPanRemoteDeviceStateMachine::IncomingConnectionFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole)
	{
	return iParent.IncomingConnectionFromPeer(aLocalRole, aRemoteRole);
	}

void CPanRemoteDeviceStateMachine::SendRoleRequest(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole)
/**
Send a role request to the remote device
@param aLocalRole The proposed local role
@param aRemoteRole The proposed remote role
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	LOG3(_L("RemDev[%x]: - sending role request, local: %x, remote: %x"), this, aLocalRole, aRemoteRole);
	__ASSERT_ALWAYS(iBnepChannelController != TNodeId::NullId(), PanAgentPanic(ECallToBnepChannelBeforeChannelSetup));
	__ASSERT_DEBUG(aLocalRole!=EPanRoleUnknown, PanAgentPanic(ESendingUnknownRoleInRoleRequest));
	__ASSERT_DEBUG(aRemoteRole!=EPanRoleUnknown, PanAgentPanic(ESendingUnknownRoleInRoleRequest));

	TPanMessage::TRoleRequestFromLocalDevice msg(aLocalRole, aRemoteRole);
    RClientInterface::OpenPostMessageClose(Id(),iBnepChannelController,msg);

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	__ASSERT_ALWAYS(iBnepChannelController, PanAgentPanic(ECallToBnepChannelBeforeChannelSetup));
	__ASSERT_DEBUG(aLocalRole!=EPanRoleUnknown, PanAgentPanic(ESendingUnknownRoleInRoleRequest));
	__ASSERT_DEBUG(aRemoteRole!=EPanRoleUnknown, PanAgentPanic(ESendingUnknownRoleInRoleRequest));
	
	LOG3(_L("RemDev[%x]: - sending role request, local: %x, remote: %x"), this, aLocalRole, aRemoteRole);

	iBnepChannelController->BnepRoleRequestFromLocalDevice(aLocalRole, aRemoteRole);
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	}
	
void CPanRemoteDeviceStateMachine::SendRoleResponse(TBnepSetupConnectionResponseMessage aRoleResponseCode)
/**
Send a role reponse to the remote device
@param aRoleReponseCode A BNEP error code indicating the status of the request
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	__ASSERT_ALWAYS(iBnepChannelController != TNodeId::NullId(), PanAgentPanic(ECallToBnepChannelBeforeChannelSetup));

	TPanMessage::TRoleResponseFromLocalDevice msg(aRoleResponseCode);
    RClientInterface::OpenPostMessageClose(Id(),iBnepChannelController,msg);
#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

	__ASSERT_ALWAYS(iBnepChannelController, PanAgentPanic(ECallToBnepChannelBeforeChannelSetup));
	iBnepChannelController->BnepRoleResponseFromLocalDevice(aRoleResponseCode);
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	}
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Get BNEP to create a new BNEP level connection for this remote device
*/
void CPanRemoteDeviceStateMachine::CreateNewBnepConnection(RInternalSocket& aConnectedSocket, TPanMessage::TPanActivity aActivity)
	{
	__ASSERT_DEBUG(iBnepChannelController == TNodeId::NullId(), PanAgentPanic(EChannelControllerAlreadyExists));

    TPanMessage::TCreateChannelController msg(&aConnectedSocket);
    RClientInterface::OpenPostMessageClose(TNodeCtxId(aActivity,Id()),iParent.BnepConnectionManager(),msg);
	}

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
Get BNEP to create a new BNEP level connection for this remote device
*/
void CPanRemoteDeviceStateMachine::CreateNewBnepConnectionL(RInternalSocket& aConnectedSocket)
	{
	__ASSERT_DEBUG(!iBnepChannelController, PanAgentPanic(EChannelControllerAlreadyExists));
	iBnepChannelController = &(iParent.BnepConnectionManager().NewBnepConnectionL(aConnectedSocket, *this));
	}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		
void CPanRemoteDeviceStateMachine::DisconnectBnepChannel()
/**
Tell the BNEP channel to disconnect; if there is no BNEP channel, just return
*/
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if(iBnepChannelController != TNodeId::NullId())
		{
		TPanMessage::TCloseChannelController msg;
        RClientInterface::OpenPostMessageClose(Id(),iBnepChannelController,msg);
		iBnepChannelController = TNodeId::NullId();
		}

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if(iBnepChannelController)
		{
		iBnepChannelController->Close(); // shutdown the connection if it exists and detach this object from BNEP
		iBnepChannelController = NULL;
		}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	}

void CPanRemoteDeviceStateMachine::SetUplinkAccessAllowed(TBool aAllowed)
	{
	// Nothing to be done if the argument matches the current state.
	if(aAllowed != iUplinkAccessAllowed)
		{
		if(aAllowed)
			{
			// Close any existing connection that has access to the uplink
			// THIS SHOULD BE REMOVED WHEN >1 REMOTE DEVICE IS ALLOWED TO ACCESS THE UPLINK.
			iParent.CloseExistingConnectionWithUplinkAccess();
			}
			
		iUplinkAccessAllowed = aAllowed;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		// Inform BNEP that packets can be forwarded to the uplink
		TPanMessage::TSetUplinkAccessAllowedForBnepLink msg(iUplinkAccessAllowed);
        RClientInterface::OpenPostMessageClose(Id(),iBnepChannelController,msg);

#else
		// Inform BNEP that packets can be forwarded to the uplink
		iBnepChannelController->SetUplinkAccessAllowedForBnepLink(iUplinkAccessAllowed);
#endif
		}
	}

TBool CPanRemoteDeviceStateMachine::UplinkAccessAllowed() const
	{
	return iUplinkAccessAllowed;
	}

void CPanRemoteDeviceStateMachine::SetState(CPanRemDevStateBase& aState)
/**
Change the current state to the one provided
*/
	{
	iState = &aState;
	TRAPD(err, iState->OnEntryL());
	if(err)
		{
		Cancel();	// cancel any outstanding async requests that states may have had running before shutting down
					// note - not all states use async requests, so this may have no effect
		Shutdown(err);
		}
	}
	
RBTPhysicalLinkAdapter& CPanRemoteDeviceStateMachine::PhysicalLinkAdapter()
	{
	return iPhysicalLinkAdapter;
	}
	
void CPanRemoteDeviceStateMachine::OpenPhysicalLinkAdapterL()
	{
	TBTDevAddr addr = RemSockAddr().BTAddr();
	User::LeaveIfError( iPhysicalLinkAdapter.Open(iSockServ, addr) );
	}

void CPanRemoteDeviceStateMachine::Shutdown(TInt aErr)
/**
Shutdown the connection using the preallocated shutdown state
@note This can either be because an unrecoverable error occured, or just a normal shutdown
*/
	{
	__ASSERT_ALWAYS(iShutdownState, PanAgentPanic(EDoubleShutdownAttemptOnDevice));
	
	iError = aErr;
	
	// set the member pointer to NULL to indicate that we've gone to the shutdown state
	CPanRemDevStateBase* shutdownState = iShutdownState; // take a copy of the pointer, so we can NULLify the one in the class
	iShutdownState = NULL; // shutdown is about to be the active state, so drop the second pointer to it
	
	// if the state has left, then it won't have performed the setup of the next state correctly
	// this code performs the same actions to cleanup the old state and activate the new, but in
	// reverse order - otherwise we'd lose the pointer to the old state when we activated the new
	delete iState;
	iState = NULL;
	SetState(*shutdownState);
	}

void CPanRemoteDeviceStateMachine::GetExistingConnections(TPanConnectionList& aPanConnectionList)
	{
	iParent.GetExistingConnections(aPanConnectionList);
	}
	
		
//
// State base class implementations
//

CPanRemDevStateBase::CPanRemDevStateBase(MPanRemDevStateMachineNotify& aStateMachine, TPanRemDevStates aStateNumber) :
#ifdef _DEBUG
	iStateMachine(aStateMachine), iStateNumber(aStateNumber)
#else
	iStateMachine(aStateMachine), iStateNumber(aStateNumber)
#endif // _DEBUG
/**
Stash the state machine handle in our variable
*/
	{	}

CPanRemDevStateBase::~CPanRemDevStateBase()
/**
Nothing to do here...
*/
	{	}

/**
Calls the appropriate panic function to encode the panic
code with the current state identifier.
@param aPanic The panic code that the state is panicking with.
*/
void CPanRemDevStateBase::PanicInState(TPanAgentPanic aPanic) const
	{
	PanAgentPanic(aPanic, iStateNumber);
	}

void CPanRemDevStateBase::BnepRoleRequestFromRemoteDeviceL(const TUUID& /*aRequestedLocalRole*/, const TUUID& /*aRequestedRemoteRole*/)
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EInvalidBnepRoleRequestReceived);
	}

void CPanRemDevStateBase::BnepRoleResponseFromRemoteDeviceL(TBnepSetupConnectionResponseMessage /*aRoleResponseCode*/)
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EInvalidBnepRoleResponseReceived);
	}
		
void CPanRemDevStateBase::RemoteDeviceDisconnectL(TInt /*aError*/)
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EInvalidRemoteDeviceDisconnectReceived);
	}
	
void CPanRemDevStateBase::ReadyForRoleRequestL()
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EInvalidReadyForRoleRequestReceived);	
	}
	
void CPanRemDevStateBase::AsyncEventCompleteL()
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EUnexpectedAsyncEventReceivedByState);
	}
	
void CPanRemDevStateBase::AsyncEventCancelL()
/**
Base class implementation - should never be called
*/
	{
	PanicInState(EUnexpectedAsyncEventReceivedByState);
	}

TInt CPanRemDevStateBase::PerformMasterSlaveSwitchIfNecessary(TBluetoothPanRole aLocalRole)
/**

*/
	{
	TInt rerr = KErrNone;
	__ASSERT_ALWAYS((aLocalRole==EPanRoleU)||(aLocalRole==EPanRoleGn)||(aLocalRole==EPanRoleNap), PanicInState(ETryingToDecideWhetherToBecomePiconetMasterBasedOnInvalidRole));
	
	if(aLocalRole==EPanRoleU)
		{
		LOG1(_L("RemDevSB: ...local role %x, don't need to be master"), aLocalRole);
		}
	else	// we *must* be piconet master
		{
		// Check the current Master Slave role for this physical link.
		TUint32 currentPhyState;
		TInt err = StateMachine().PhysicalLinkAdapter().PhysicalLinkState(currentPhyState);
		LOG2(_L("PerformMasterSlaveSwitchIfNecessary: PhysicalLinkState Err = %d, Link State = %x"), 
		                   err, currentPhyState);

		// If the current PHY state was retrieved successfully and the state indicates that
		// this device is not currently master of the PHY then attempt to switch roles.
		if(err == KErrNone && !(currentPhyState & ENotifyMaster))
			{
			rerr = BecomePiconetMaster();
			}
		else
			{
			// The represents the following logic:
			// If retrieval of PHY state succeed but the local device is already master then
			// return KErrNone.
			// Otherwise return the failure reason from the PhysicalLinkState method.
			rerr = err;
			}
		}
		
	return rerr;	
	}

void CPanRemDevStateBase::ConvertUuidsToPanRolesL(const TUUID& aLocalRoleUuid, const TUUID& aRemoteRoleUuid, TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole)
/**
Convert the supplied UUIDs to TBluetoothPanRoles
@note If this method leaves with an invalid UUID error, it will have sent a response to the 
remote device with the appropriate failure code
@param aLocalRoleUuid The local role UUID
@param aRemoteRoleUuid The remote role UUID
@param aLocalRole On return, contains the local PAN role UUID in TBluetoothPanRole form
@param aRemoteRole On return, contains the remote PAN role UUID in TBluetoothPanRole form
@leave KErrInvalidDestinationServiceUuid, if the destination service UUID is invalid; KErrInvalidSourceServiceUuid if the source service UUID is invalid
*/
	{
	TInt err;
	err = UuidToPanRole(aLocalRoleUuid, aLocalRole);
	if(err)
		{
		LOG(_L("RemDevState: <[base]> - sending invalid destination service UUID error."));
		StateMachine().SendRoleResponse(EInvalidDestinationServiceUuid);
		User::Leave(KErrInvalidDestinationServiceUuid);
		}
		
	err = UuidToPanRole(aRemoteRoleUuid, aRemoteRole);
	if(err)
		{
		LOG(_L("RemDevState: <[base]> - sending invalid source service UUID error."));
		StateMachine().SendRoleResponse(EInvalidSourceServiceUuid);
		User::Leave(KErrInvalidSourceServiceUuid);
		}
	}
	
TInt CPanRemDevStateBase::UuidToPanRole(const TUUID& aUuid, TBluetoothPanRole& aRole)
/**
Convert the given UUID into a TBluetoothPanRole
@param aUuid The UUID to convert
@param aRole If successful, on return contains the UUID in TBluetoothPanRole form
@return KErrNone if successful; KErrArgument if the supplied UUID is an invalid PAN role
*/
	{
	TInt err;
	
	TPtrC8 ptr(aUuid.ShortestForm());
	
	TUint16 panRole = static_cast<TUint16>(EPanRoleUnknown);
	if(ptr.Length()!=2)
		{
		err = KErrArgument;
		}
	else
		{
		panRole = static_cast<TUint16>((ptr.Ptr()[0] << 8) | ptr.Ptr()[1]);
		err = KErrNone;	
		}
	
	aRole = static_cast<TBluetoothPanRole>(panRole);

	if(err || ((aRole!=EPanRoleU)&&(aRole!=EPanRoleGn)&&(aRole!=EPanRoleNap))) //@note Embeds knowledge of valid PAN roles
		{
		LOG(_L("RemDevSB: ...could not convert to PAN role, returning KErrArgument."));
		return KErrArgument;		
		}
	else
		{
		LOG1(_L("RemDevSB: ...converted to PAN role %x"), panRole);
		return KErrNone;
		}
	}

TInt CPanRemDevStateBase::BecomePiconetMaster()
/**
Become the master of the piconet.
Code from RBTBaseband
@return KErrNone if successful; KErrWaitingForBasebandRoleSwitch if role switch is necessary and in progress; otherwise a Symbian standard error code
*/
	{
#ifdef _DEBUG
	TBTDevAddr KNullDevAddr(MAKE_TINT64(0x0000, 0x00000000));
	__ASSERT_DEBUG(StateMachine().RemSockAddr().BTAddr()!=KNullDevAddr, PanicInState(ENullRemoteDeviceAddress));
#endif

	
	TInt err = StateMachine().PhysicalLinkAdapter().RequestMasterRole();
	return (err == KErrNone ? KErrWaitingForBasebandRoleSwitch : err);
	}
	
MPanRemDevStateMachineNotify& CPanRemDevStateBase::StateMachine()
/**
Return the handle to the state machine
*/
	{
	return(iStateMachine);
	}



