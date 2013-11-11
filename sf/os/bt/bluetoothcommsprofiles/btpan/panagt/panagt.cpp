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

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include <comms-infras/nifif.h> // should be removed once KCOGetAgentEMIPtr is defined in nifagt.h
#include <elements/nm_interfaces.h>
#include "panagtlog.h"
#include "panagt.h"
#include "panagtremdev.h"
#include "panagtstates.h"
#include "panlocalsdphelper.h"
#include <comms-infras/nifagt_internal.h>
#include <panctrl.h>
#include <btextnotifierspartner.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include "panprovisioninfo.h" 
#include "panmessages.h"

#ifdef ESOCK_EXTLOG_ACTIVE
_LIT8(KPanAgtSubTag, "panagt");
#endif

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_AGENT);
#endif

using namespace PanAgent;
using namespace Messages;

extern "C" EXPORT_C CNifAgentFactory* NewAgentFactoryL()
/**
Pan agent factory
*/
	{	
	return new(ELeave) CPanAgentFactory();
	}
	
//
// PAN agent factory
//

void CPanAgentFactory::InstallL() 
/**
 * Perform any agent factory initialisation
 */
	{
	LOG_FUNC
	}
 
CNifAgentBase* CPanAgentFactory::NewAgentL(const TDesC& /*aName*/)
/**
 * Create a new PAN agent
 */
	{
	LOG_FUNC
	return CPanAgent::NewL();
	}

TInt CPanAgentFactory::Info(TNifAgentInfo& aInfo, TInt /*aIndex*/) const
/**
 * Retrieve information about this agent
 * @param aInfo 
 * @param aIndex
 */
	{
	LOG_FUNC
	aInfo.iName = KPanAgentName;
	aInfo.iVersion = TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	return KErrNone;
	}

//
// PAN agent
//

CPanAgent* CPanAgent::NewL()
/**
Return a newly constuct PAN agent
*/
	{
	LOG_STATIC_FUNC
	CPanAgent* self = new(ELeave) CPanAgent();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return(self);
	}

CPanAgent::CPanAgent()
/**
Do nothing
*/
	{
	CONNECT_LOGGER
	LOG_FUNC

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NM_LOG_NODE_CREATE(KPanAgtSubTag, CPanAgent);
#endif
	}

void CPanAgent::ConstructL()
/**

*/
	{
	LOG_FUNC
	CAgentBase::ConstructL();
	
	iDeviceSelector = new (ELeave) CPanRemoteDeviceSelector(*this, *iDatabase);
	
	iInitiateDisconnectCallback = new (ELeave) CAsyncCallBack(KPanAgtAoPriority);

	TCallBack initiateDisconnectCallback(InitiateDisconnectCb, this);
	iInitiateDisconnectCallback->Set(initiateDisconnectCallback);

	// Create local SDP helper instance.
	iPanLocalSdpRegistrar = CPanHelperLocalSdpRegistrar::NewL();

	iState = CPanAgtStateUninitialised::NewL(*this);
	iShutdownState = CPanAgtStateShutdown::NewL(*this);

	//If CPanCoexistenceConnectionController::NewL leaves, PAN still can be started as usual
	TRAP_IGNORE(iPanCoexistenceConnectionController = CPanCoexistenceConnectionController::NewL(*this));
	__ASSERT_DEBUG(iPanCoexistenceConnectionController, PanAgentPanic(ECreatePanCoexistenceConnectionControllerFailed));


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// Register with comms transport to be able to receive our
	// custom PAN/BNEP messages
	TPanMessage::RegisterL();
#endif
	}

CPanAgent::~CPanAgent()
/**
Tidy up after ourselves
*/
	{
	LOG_FUNC

	__ASSERT_DEBUG(Devices().Count()==0, PanAgentPanic(EPanAgentDeletedWithDevicesStillAttached));
	
	delete iPanLocalSdpRegistrar;
	
	iRemoteDevices.ResetAndDestroy();
	
	if (iShutdownState != iState)
		{	
		// something strange happened here, shutdown state not the last used state
		__ASSERT_DEBUG(EFalse, PanAgentPanic(EShutdownStateNotLastState));
		
		delete iShutdownState;
		iShutdownState = NULL;
		}
		
	delete iState;
	iState = NULL;
	
	if(iDeviceSelector)
		{
		iDeviceSelector->Cancel();
		delete iDeviceSelector;
		iDeviceSelector = NULL;
		}
	
	if(iPanCoexistenceConnectionController)
		{
		delete iPanCoexistenceConnectionController;
		iPanCoexistenceConnectionController = NULL;
		}
	
	if(iIncomingConnectionListener)	// if we haven't deleted it already...
		{
		// make sure we stopped it
		__ASSERT_DEBUG(!(iIncomingConnectionListener->IsActive()), PanAgentPanic(EIncomingConnectionListenerStillActiveAtShutdown));
		iIncomingConnectionListener->Cancel();	// recover in release builds
		delete iIncomingConnectionListener;
		iIncomingConnectionListener = NULL;	
		}
	
	iInitiateDisconnectCallback->Cancel();
	delete iInitiateDisconnectCallback;
	iInitiateDisconnectCallback = NULL;
	
	delete iDeviceSelector;
	iDeviceSelector = NULL;
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	// De-register with the comms transport
	TPanMessage::DeRegister();
    NM_LOG_NODE_DESTROY(KPanAgtSubTag, CPanAgent);
#endif
	CLOSE_LOGGER
	}

void CPanAgent::Connect(TAgentConnectType aType)
/**
Start the connection in an appropriate mode, depending on the settings in commdb
*/
	{
	LOG_FUNC
	LOG1(_L("CPanAgent::Connect(TAgentConnectType[%d])"), aType);

	//The IAP must be incoming if we get a reconnect
	if(aType==EAgentStartDialIn || aType==EAgentReconnect)
		{
		SetSuppressOutgoing();
		}
		
	iError = KErrNone; // make sure iError is reset before entering ConnectComplete
	iState->Connect();
	}

void CPanAgent::Connect(TAgentConnectType aType, CStoreableOverrideSettings* aOverrideSettings)
/**
Pass the override settings to base class, and perform a normal start
*/
	{
	LOG_FUNC
	LOG1(_L("CPanAgent::Connect(TAgentConnectType[%d] + CStoreableOverrideSettings)"), aType);

	TRAPD(err, SetOverridesL(aOverrideSettings));
	if(err) // if we can't store the overrides, then tell nifman and abort the connect
		{
		iError = err;
		iState->Connect();
		return;
		}
	Connect(aType);
	}

void CPanAgent::CancelConnect()
/**
Cancel the ServiceStarted callback - this will prevent the agent starting
*/
	{
	LOG_FUNC
	iState->CancelConnect();
	}
	
void CPanAgent::ServiceStarted()
/**
@note We can't handle any error conditions here, as nifman doesn't allow us to signal them until
we call ConnectComplete
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iNotify, PanAgentPanic(ENullNifmanNotifyPointer));
	LOG1(_L("CPanAgent: Service started with error %d"), iError);
	iNotify->ServiceStarted(); // trigger loading of nif and packet driver
	}

void CPanAgent::ConnectComplete()
/**
Get the BNEP control interface handle, and create the initial state
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iNotify, PanAgentPanic(ENullNifmanNotifyPointer));
	LOG1(_L("CPanAgent: entered ConnectComplete with error %d"), iError);

	if(!iError) // if everything's OK, then get the handle to the BNEP control interface
		{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        CPanProvisionInfo* provisionInfo=NULL; 
		iError = iNotify->Notification((TAgentToNifEventType)EPanAgentToNifGetPanProvisionInfo, static_cast<TAny*>(&provisionInfo));
		
		// If this assert fails check your commdb. It probably means that a non-399 commdb
		// has been imported and is using the basic Ethernet MCPR rather than the Pan
		// Ethernet MCPR
    	__ASSERT_ALWAYS(provisionInfo && provisionInfo->BnepConnectionMgr() != Messages::TNodeId::NullId(), PanAgentPanic(ENullBnepConnectionManager));
    	iBnepConnectionManager = provisionInfo->BnepConnectionMgr();
		
#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		// Get the handle to the nif's connection manager
		LOG(_L("CPanAgent: Getting handle to BNEP control interface..."));
		TBnepConnectionManager bnepConnectionManager;
		iError = iNotify->Notification((TAgentToNifEventType)EPanAgentToNifGetConnectionManager, static_cast<TAny*>(&bnepConnectionManager));
		if(iError == KErrNone)
			{
			iBnepConnectionManager = bnepConnectionManager.iBnepConnectionManager;
			__ASSERT_ALWAYS(iBnepConnectionManager, PanAgentPanic(ENullBnepConnectionManager));
			}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		}

	if (iError == KErrNone)
		{
		LOG(_L("CPanAgent: ...got handle successfully"));
		delete iState;
		iState = NULL;
		CPanAgtStateBase* initialState = NULL;
		TRAP(iError, initialState = CPanAgentStateInitialising::NewL(*this));
		if(!iError)
			{
			SetState(*initialState); // this may set iError if there is a problem during initialising		
			}
		}

	if(iError)
		{
		iNotify->AgentProgress(EPanAgtIdle, iError);
		}
	else
		{
		iNifmanConnected = ETrue;
		iNotify->AgentProgress(EPanAgtConnected, KErrNone);
		}

	LOG1(_L("CPanAgent: calling nifman ConnectComplete with error %d)"), iError);
	iNotify->ConnectComplete(iError);
	}

void CPanAgent::Reconnect()
/**
Reset state and asynchronously send reconnect complete back, if 
the IAP is incoming
*/
	{
	LOG_FUNC
	if(AllowIncoming())
		{
		delete iState;
		iState = NULL;
		CPanAgtStateBase* reconState = NULL;
		TRAP(iError, reconState= CPanAgtStateReconnect::NewL(*this));
		if(!iError)
			{
			SetState(*reconState);
			}
		}
	}

void CPanAgent::ReconnectComplete(TInt aErr)
	{
	LOG_FUNC
	iNotify->ReconnectComplete(aErr);
	}

void CPanAgent::CancelReconnect()
/**

*/
	{
	LOG_FUNC
	iState->CancelReconnect();
	}
	
void CPanAgent::InitiateDisconnect()
/**
Perform an agent initiated disconnect
@note Any code in the agent that wants to trigger a disconnect should call RequestDisconnect(),
which will call this method via an async callback.
@note This call will loop back into the agent because nifman will call the Disconnect() method itself.
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iNifmanConnected, PanAgentPanic(EAttemptToDisconnectBeforeConnectComplete));
	LOG1(_L("CPanAgent: InitiateDisconnect with error %d"), iError);
	
	iNotify->AgentProgress(EPanAgtDisconnecting, iError);
	iNotify->AgentEvent(EAgentOriginatedConnectionCommand, EAgentConnectionCommandStop, TPckg<TInt>(KErrDisconnected));
	}
	
void CPanAgent::Disconnect(TInt aReason)
/**
Disconnect called from nifman (this can be triggered by the idle timers, the client, or us 
requesting a disconnect using RequestDisconnect()).
This method will cause us to go to the shutdown state in the role state machine, which will
shut everything else down (ie. the remote device state machines).
@note This is the one case where a state transition can occur that is not triggered by a state
itself (because two of the three reasons for this method being called are events external to 
PAN agent, and those events turn up here first, and are not passed to a state; instead they 
trigger the state transition directly)
*/
	{
	LOG_FUNC
	LOG1(_L("CPanAgent: Disconnect with error %d"), aReason);

	iError = aReason;
	
	// now make shutdown the active state
	delete iState; // we have to do this now, otherwise we lose the pointer
	iState = NULL; // to prevent any mistakes being made...
	SetState(*iShutdownState); // shutdown is complete when ShutdownComplete() is called by the shutdown state	
	}
	
void CPanAgent::FullShutdownComplete()
/**
Once all devices have reported to the shutdown state that they are disconnected, this method
is called and the agent can disconnect
*/
	{
	LOG_FUNC
	LOG(_L("CPanAgent: Shutdown complete, signalling nifman with DisconnectComplete()"));

	// then tell nifman we've disconnected
	iNotify->AgentProgress(EPanAgtDisconnected, KErrNone);
	iNifmanConnected = EFalse;
	iNotify->DisconnectComplete();
	}
		
TInt CPanAgent::InitiateDisconnectCb(TAny* aThisPtr)
/**
Initiate disconnect callback static method
*/
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aThisPtr, PanAgentPanic(ENullTAnyPointer));

	CPanAgent* self = static_cast<CPanAgent*>(aThisPtr);
	self->InitiateDisconnect();
	return KErrNone;
	}
	
void CPanAgent::Info(TNifAgentInfo& aInfo) const
/**
 * Information about this Agent
 *
 * @param aInfo on return contains information about the agent
 */
	{
	LOG_FUNC
	aInfo.iName = KPanAgentName;
	aInfo.iName.AppendFormat(_L("-AgentFactory[0x%08x]"), this);
	aInfo.iVersion = TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
	}

TInt CPanAgent::Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption)
/**
Various agent specific options
*/
	{
	LOG_FUNC
	if(aOptionLevel==KCOLAgent)
		{
		TBool listUplinkAccessDev = EFalse;
		
		switch(aOptionName)
			{
			case KCOGetAgentEMIPtr:	// agent extended management interface getter
				{
				if(aOption.Length()!=sizeof(MNifAgentExtendedManagementInterface*))
					{
					return KErrArgument;
					}
				aOption = TPckg<MNifAgentExtendedManagementInterface*>(this);
				return KErrNone;
				}
				
			case KCOAgentNotifyClientAttach:
				{
				TRAPD(err, SetupOutgoingConnectionsL()); //<@note This ignores the value of the iSuppressOutgoing flag and triggers the outgoing connections directly
														 // as the outgoing connection suppression is only intended to be used when the agent is first told to Connect()
														 // and not when the 2nd and subsequent clients attach to this connection
				return err;
				}
				
			case KCOAgentPanConnectDevice:
				{
				if(aOption.Length() != KBTDevAddrSize)
					{
					return KErrArgument;
					}
				
				TBTDevAddr remoteDeviceAddress(aOption);
				TRAPD(err, CreateNewConnectionForOutgoingL(remoteDeviceAddress));
				return err;
				}
				
			case KCOAgentPanDisconnectDevice:
				{
				if(aOption.Length() != KBTDevAddrSize)
					{
					return KErrArgument;
					}
				
				TBTDevAddr remoteDeviceAddress(aOption);
				CPanRemoteDeviceStateMachine* remoteDevice = FindRemoteDeviceByAddress(remoteDeviceAddress);
				if(remoteDevice)
					{
					remoteDevice->Disconnect();
					}
				else
					{
					return KErrNotFound;
					}
					
				return KErrNone;
				}
				
			case KCOAgentPanEnumerateUplinkAccessDevices: 
				{
				//set the listing uplink access devices boolean to ETrue
				listUplinkAccessDev = ETrue;
				//the following code will branch based on the value of listUplinkAccessDev
				//fall through
				}
				
			case KCOAgentPanEnumerateDevices:
				{
				// Because the array passed to us here may be smaller than the number of active links
				// at this point, we shall return as much as possible in the array.

				// Also if the array has enough space for more addresses than we currently have, the
				// descriptor size will be changed to reflect reality.

				aOption.Zero();

				TInt maxDevices = Min ( iRemoteDevices.Count(),
										aOption.MaxLength() / sizeof(TBTDevAddr));
				
				TInt currentDevice = 0;
				TInt devicesStored = 0; // We only enumerate fully connected devices, so we need a separate counter
								
				while ((currentDevice < iRemoteDevices.Count()) && (devicesStored < maxDevices))
					{
					TRemoteDeviceState state = iRemoteDevices[currentDevice]->GetState();
					
					if(!listUplinkAccessDev && state != EIdle && state != EDisconnecting || 
						iRemoteDevices[currentDevice]->UplinkAccessAllowed())
						{
						TBTDevAddr devAddr = iRemoteDevices[currentDevice]->DevAddr();
						TPckg<TBTDevAddr> pckg(devAddr);
						aOption.Append(pckg);
						devicesStored++;
						}
					
					currentDevice++;
					}
				
				return KErrNone;
				}
							
			default:
				{
				return KErrNotSupported;
				}
			}
		}
	return KErrNotSupported;
	}

TInt CPanAgent::Notification(TNifToAgentEventType /*aEvent*/, TAny* /*aInfo*/)
/**

*/
	{
	LOG_FUNC
	return KErrNotSupported;
	}

TInt CPanAgent::GetExcessData(TDes8& /*aDes*/)
/**
Unused in PAN profile
*/
	{
	LOG_FUNC
	return KErrNotSupported;
	}

TInt CPanAgent::IncomingConnectionReceived()
/**
Don't need to support this - it's for an older nif/agent model where incoming and outgoing connections can't be supported at the same time.  This isn't the case in PAN profile.
*/
	{
	LOG_FUNC
	return KErrNotSupported;
	}

void CPanAgent::GetLastError(TInt& aError)
	{
	LOG_FUNC
	aError = KErrNotSupported; 
	}

TBool CPanAgent::IsReconnect() const
	{
	LOG_FUNC
	//Return false for now because it hasn't been used. We will only expose this information from 
	//statemachine when there is a good reason.
	__DEBUGGER();
	return EFalse; 	
	}

void CPanAgent::MpccoPanCoexistenceConnectionUpdated(TIPBearerCoexistenceStatus aStatus)
	{
	LOG_FUNC
	
	switch (aStatus)
		{
		case ENoneIsActive:
			{
			// We will not register the sdp and open listen socket in shutdown state
			// Also check that we are not already listening
			if (iShutdownState != iState && !IsIncomingConnectionListenerEnabled())
				{
				TRAPD(err, iState->StartListenerL());
				if(err)
					{
					LOG1(_L("CPanAgent: MpccoPanCoexistenceConnectionUpdated open listener left with %d, shutting down all connections"), err);
					// If open listener failed, request to disconnect
					RequestDisconnect(err);
					}
				}
			}
			break;

		case ERndisIsActive:
			// Close listener and unregister SDP record since RNDIS is active
			iState->StopListener();
			break;

		case EBTPanIsActive:
			// It maybe some funny things happened here
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EIPBearerCoexistancePropertyShouldnotBePanIsActive));
			break;

		default:
			__ASSERT_DEBUG(EFalse, PanAgentPanic(EInvalidIPBearerCoexistanceProperty));
			break;
		}
	}
//
// PAN agent accessor methods
//

RPointerArray<CPanRemoteDeviceStateMachine>& CPanAgent::Devices()
/**
Return a handle to the connection array
@return A reference to the device array held by the state machine
*/
	{
	LOG_FUNC
	return(iRemoteDevices);
	}

const RPointerArray<CPanRemoteDeviceStateMachine>& CPanAgent::Devices() const
/**
Return a const handle to the connection array
@return A const reference to the device array held by the state machine
*/
	{
	LOG_FUNC
	return(iRemoteDevices);
	}
	
TBluetoothPanRole& CPanAgent::FixedLocalRole()
/**
Return a handle to the fixed local role field
@return A reference to the fixed local role field in the state machine
*/
	{
	LOG_FUNC
	return(iFixedLocalRole);
	}

TBluetoothPanRole& CPanAgent::FixedRemoteRole()
/**
Return a handle to the fixed remote role field
@return A reference to the fixed remote role field in the state machine
*/
	{
	LOG_FUNC
	return(iFixedRemoteRole);
	}
		
CCommsDbAccess& CPanAgent::Database()
/**
Return a handle to commdb
@return A handle to commdb, with overrides masking the appropriate fields
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iDatabase, PanAgentPanic(ENullCommdbHandle));
	
	return(*iDatabase);
	}

CPanHelperLocalSdpRegistrar& CPanAgent::PanLocalSdpRegistrar()
/**

*/
	{
	LOG_FUNC
	return *iPanLocalSdpRegistrar;
	}
	
TInt CPanAgent::IapId()
/**
Return the IAP ID of this connection
*/
	{
	LOG_FUNC
	TUint32 iapId = 0;	// initialise to zero to keep VC6 compiler happy...
	TRAPD(err, Database().GetIntL(TPtrC(IAP), TPtrC(COMMDB_ID), iapId));
	if(err)
		{
		LOG1(_L("CPanAgent: could not retrieve IAP ID, error %d"), err);
		return err;
		}
	else
		{
		return iapId;
		}
	}

TBool CPanAgent::SuppressOutgoing() const
/**
Find out if outgoing connections are suppressed
*/
	{
	LOG_FUNC
	return(iSuppressOutgoing);
	}
	

//
// PAN agent do-er methods
//

void CPanAgent::SetSuppressOutgoing()
/**
Suppress outgoing connections
*/
	{
	LOG_FUNC
	iSuppressOutgoing = ETrue;
	}

void CPanAgent::ResetSuppressOutgoing()
/**
Stop outgoing connections being suppressed
*/
	{
	LOG_FUNC
	iSuppressOutgoing = EFalse;
	}

void CPanAgent::SendProgress(TInt aProgress, TInt aError)
/**
Send progress values to nifman
NOTE nifman will ditch duplicate notifications, same progress, same error
We can't change this so to avoid this for connection and disconnection progress notifications
we will send a role notification after the connect/disconnect
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iNotify, PanAgentPanic(ENullNifmanNotifyPointer));

	LOG2(_L("CPanAgent: sending progress value to connection; stage %d, error %d"), aProgress, aError);
	iNotify->AgentProgress(aProgress, aError);

	switch (aProgress)
	{
	case EPanAgtInitialising:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtInitialising"));
		break;
		}
	case EPanAgtConnected:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtConnected"));
		break;
		}
	case EPanAgtIdle:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtIdle"));
		break;
		}
	case EPanAgtListening:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtListening"));
		break;
		}
	case EPanAgtURole:
		{
		// DANGER Don't EVER put StateSendRoleNotification in here - endless loop
		LOG(_L("CPanAgent: Progress value EPanAgtURole"));
		break;
		}
	case EPanAgtGnRole:
		{
		// DANGER Don't EVER put StateSendRoleNotification in here - endless loop
		LOG(_L("CPanAgent: Progress value EPanAgtGnRole"));
		break;
		}
	case EPanAgtNapRole:
		{
		// DANGER Don't EVER put StateSendRoleNotification in here - endless loop
		LOG(_L("CPanAgent: Progress value EPanAgtNapRole"));
		break;
		}
	case EPanAgtConnectedNewDevice:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtConnectedNewDevice"));
		// Get the state to send a role notification if its in a sensible state
		// This has 2 beneficial effects.
		// a) We send the role to the layers above so they know
		// b) We never have the situation where we are sending 2 connects or 2 disconnects
		//	   one after the other, this stops comms filtering the notification out
		iState->StateSendRoleNotification();
		
		break;
		}
	case EPanAgtDisconnectedDevice:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtDisconnectedDevice"));
		// Get the state to send a role notification if its in a sensible state
		// This has 2 beneficial effects.
		// a) We send the role to the layers above so they know
		// b) We never have the situation where we are sending 2 connects or 2 disconnects
		//	   one after the other, this stops comms filtering the notification out
		iState->StateSendRoleNotification();
		
		break;
		}
	case EPanAgtReconfiguringPiconet:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtReconfiguringPiconet"));
		break;
		}
	case EPanAgtUplinkRequired:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtUplinkRequired"));
		break;
		}
	case EPanAgtUplinkNotRequired:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtUplinkNotRequired"));
		break;
		}
	case EPanAgtDisconnecting:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtDisconnecting"));
		break;
		}
	case EPanAgtDisconnected:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtDisconnected"));
		break;
		}
	case EPanAgtNoResourceForPanConnection:
		{
		LOG(_L("CPanAgent: Progress value EPanAgtNoResourceForPanConnection"));
		// We are lack of the network resource to setup the PAN connection, mainly caused by the RNDIS which
		// is already occupied the IP resource
		break;
		}
	default:
		{
		LOG(_L("CPanAgent: Progress value UNKNOWN"));
		break;
		}			
	}

	LOG(_L("CPanAgent: <<Progress SENT"));
	}
	

TInt CPanAgent::StartIncomingConnectionListener()
/**
Start the incoming connection listener
@return KErrNone if successful; otherwise one of the Symbian standard error codes
*/
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(!iIncomingConnectionListener)
		{
		TRAP(rerr, iIncomingConnectionListener = CPanIncomingConnectionListener::NewL(*this));
		}
	return rerr;
	}
	
void CPanAgent::CloseIncomingConnectionListener()
/**
Stop and delete the incoming connection listener
*/
	{
	LOG_FUNC
	if(!iIncomingConnectionListener)	// if the incoming connection listener isn't active, then return immediately
		{
		return;
		}
		
	iIncomingConnectionListener->Cancel();	// cancel outstanding accept
	delete iIncomingConnectionListener;
	iIncomingConnectionListener = NULL;
	}
	
void CPanAgent::SetupOutgoingConnectionsL()
/**
Begin the process of setting up any outgoing connections that we need
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iDeviceSelector, PanAgentPanic(ENullDeviceSelector));
	
	LOG(_L("Agent: Asked to bring new device(s) into the PAN"));
	if (iRemoteDevices.Count())
		{
		TBluetoothPanRole remoteRole = iRemoteDevices[0]->RemoteRole();
		if (remoteRole == EPanRoleNap)
			{
			LOG(_L("Agent: Already connected to a NAP, aborting."));
			return;
			}
		}

	// start the process of connecting any remote devices
	iDeviceSelector->PerformRemoteDeviceSelectionL();
	}
		
//
// PAN agent getter methods
//

TBool CPanAgent::IsIncomingConnectionListenerEnabled() const
/**
Is the incoming connection listener active?
@return Whether the listener is active or not
*/
	{
	LOG_FUNC
	return (iIncomingConnectionListener ? ETrue : EFalse);
	}

TBool CPanAgent::IsPromptingUserForDeviceSelection() const
/**
Is the user being shown the device selection notifier?
*/
	{
	LOG_FUNC
	return (iDeviceSelector ? iDeviceSelector->IsPrompting() : EFalse);
	}

TBool CPanAgent::IsUAllowedByFixedLocalRole() const
/**
Is U role allowed by settings in commdb?
*/
	{
	LOG_FUNC
	return ((iFixedLocalRole==EPanRoleUnknown) || (iFixedLocalRole==EPanRoleU));
	}
	
TBool CPanAgent::IsGnAllowedByFixedLocalRole() const
/**
Is GN role allowed by settings in commdb?
*/
	{
	LOG_FUNC
	return ((iFixedLocalRole==EPanRoleUnknown) || (iFixedLocalRole==EPanRoleGn));
	}
	
TBool CPanAgent::IsNapAllowedByFixedLocalRole() const
/**
Is NAP role allowed by settings in commdb?
@note For now, because we don't support NAP, we're just going to return false here
*/
	{
	LOG_FUNC
	return EFalse;
	}

//
// PAN agent setter methods	
//

void CPanAgent::SetState(CPanAgtStateBase& aState)
/**
Set the current state in the state machine
*/
	{
	LOG_FUNC
	iState = &aState;
	TRAPD(err, iState->OnEntryL());
	if(err)
		{
		LOG1(_L("CPanAgent: OnEntry to a state left with %d, shutting down all connections"), err);
		RequestDisconnect(err);
		}
	}

//
// Connection creator methods
//

void CPanAgent::CreateNewConnectionFromIncomingL(RInternalSocket& aConnectedSocket)
/**
Create a new connection from an incoming socket
@note All the role negotiation for this new connection is handled by the PAN remote connection class
@param aConnectedSocket A connected L2CAP socket, ready for hand off to BNEP
@return KErrNone if successful; otherwise one of the Symbian standard error codes
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iDatabase, PanAgentPanic(ENullCommdbHandle));
	
	TBTSockAddr sockAddr;
	aConnectedSocket.RemoteName(sockAddr);
	CheckDeviceIsNotAlreadyAttachedL(sockAddr.BTAddr());
	CPanRemoteDeviceStateMachine* newRemDev = CPanRemoteDeviceStateMachine::NewL(aConnectedSocket, *this, *iDatabase);
	CleanupStack::PushL(newRemDev);
	User::LeaveIfError(iRemoteDevices.Append(newRemDev));
	CleanupStack::Pop(newRemDev);
	}
	
void CPanAgent::CreateNewConnectionForOutgoingL(const TBTDevAddr& aRemDevAddr)
/**
Create a new connection to a remote device
@note Can be called from outgoing connection creator, or from Control() call
@param aRemDevAddr The address of the remote device to connect to
*/
	{	
	LOG_FUNC

	// Check that a new PAN connection can be established at this time.
	TInt err = TryToCreateNewPanConnection(); 
	
	if (err != KErrNone)
		{
        LOG1(_L("CPanAgent::CreateNewConnectionForOutgoingL - TryToCreateNewPanConnection returns %d"), err);
		
		// No resource is available and send a progress notification
		SendProgress(EPanAgtNoResourceForPanConnection, err);
		User::Leave(err);
		}
	
	CheckDeviceIsNotAlreadyAttachedL(aRemDevAddr);
	CPanRemoteDeviceStateMachine* newRemDev = CPanRemoteDeviceStateMachine::NewL(aRemDevAddr, *this, *iDatabase);	
	CleanupStack::PushL(newRemDev);
	User::LeaveIfError(iRemoteDevices.Append(newRemDev));
	CleanupStack::Pop(newRemDev);
	}

TInt CPanAgent::DeviceSelectionError(TInt aErr)
/**
There has been an error during remote device selection
@note This might break if the first device from set of devices causes an error, but the rest
would start correctly if given a chance. However, it's unlikely - any error affecting one device
is likely to affect them all
*/
	{
	LOG_FUNC
	LOG1(_L("CPanAgent: outgoing connection starter signalled error %d..."), aErr);	
	// should this be passed on to the state?
	// Maybe, it depends on how complicated we want to make the logic - currently states
	// have very limited knowledge of the connection starter (only in the state base class) - 
	// perhaps we should keep it that way?
	if(iRemoteDevices.Count())	// there are other devices in the array, so carry on
		{
		LOG(_L("CPanAgent: ...devices in array, ignoring error"));
		return KErrNone;	
		}
	else					// no other devices in the array, so check we're not listening, then shutdown
		{
		if(IsIncomingConnectionListenerEnabled())	// do nothing
			{
			LOG(_L("CPanAgent: ...no devices in array, but incoming connection listener active, ignoring error"));
			return KErrNone;
			}
		else
			{
			LOG(_L("CPanAgent: ...no devices in array and no incoming connection listener active, shutting down"));
			RequestDisconnect(aErr);
			return KErrDisconnected;
			}
		}
	}

//
// Methods handled by each state
//


TInt CPanAgent::InitiateOutgoingConnection(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));

	TRAPD(err, iState->InitiateOutgoingConnectionL(aLocalRole, aRemoteRole, aWorthTryingRemoteRoles));
	return err;
	}
	
TInt CPanAgent::PerformLocalRoleChangeRequest(TBluetoothPanRole& aLocalRole, TBluetoothPanRole& aRemoteRole)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));

	TRAPD(err, iState->PerformLocalRoleChangeRequestL(aLocalRole, aRemoteRole));
	return err;
	}

TInt CPanAgent::RoleChangeRequestFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));
	
	TInt ret = iState->IsRoleRequestValid(aLocalRole, aRemoteRole);
	if(ret == KErrNone)
		{
		// Check that the local service requested is currently supported.
		if((iNapEnabled && aLocalRole == EPanRoleGn) ||
		   (!iNapEnabled && aLocalRole == EPanRoleNap) )
			{
			ret = KErrInvalidOrUnacceptableRoleCombination;
			}
		// Check that the request is permitted by the fixed roles
		else if((FixedLocalRole() != EPanRoleUnknown &&
		         FixedLocalRole() != aLocalRole) ||
		        (FixedRemoteRole() != EPanRoleUnknown &&
		         FixedRemoteRole() != aRemoteRole))
			{
			ret = KErrInvalidOrUnacceptableRoleCombination;
			}
		else
			{
			// Check that the connection can co-exist with current connections and that the
			// state machine can accept the conection at this time.
			TRAP(ret, iState->RoleChangeRequestFromPeerL(aLocalRole));
			}
		}
	return ret;					
	}

TInt CPanAgent::IncomingConnectionFromPeer(TBluetoothPanRole aLocalRole, TBluetoothPanRole aRemoteRole)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));
	
	// Check that a new PAN connection can be established at this time.
	TInt ret = TryToCreateNewPanConnection(); 
	
	if (ret != KErrNone)
		{
		LOG1(_L("CPanAgent::IncomingConnectionFromPeer - TryToCreateNewPanConnection returns %d"), ret);
		
		// No resource is available and send a progress notification
		SendProgress(EPanAgtNoResourceForPanConnection, ret);
		}
	else
		{
		ret = iState->IsRoleRequestValid(aLocalRole, aRemoteRole);
		
		if(ret == KErrNone)
			{
			// Check that the local service requested is currently supported.
			if((iNapEnabled && aLocalRole == EPanRoleGn) ||
			   (!iNapEnabled && aLocalRole == EPanRoleNap) )
				{
				ret = KErrInvalidOrUnacceptableRoleCombination;
				}
			// Check that the request is permitted by the fixed roles
			else if((FixedLocalRole() != EPanRoleUnknown &&
			         FixedLocalRole() != aLocalRole) ||
			        (FixedRemoteRole() != EPanRoleUnknown &&
			         FixedRemoteRole() != aLocalRole))
				{
				ret = KErrInvalidOrUnacceptableRoleCombination;
				}
			else
				{
				// Check that the connection can co-exist with current connections and that the
				// state machine can accept the conection at this time.
				TRAP(ret, iState->IncomingConnectionFromPeerL(aLocalRole));
				}
			}
		}
	return ret;					
	}
	
void CPanAgent::DeviceActive(CPanRemoteDeviceStateMachine& aNewConnection)
/**
Indicates that the given device has transitioned to the active state
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));

	TRAPD(err, iState->DeviceActiveL(aNewConnection));	
	if(err)
		{
		LOG1(_L("CPanAgent: DeviceActive left with %d"), err);
		RequestDisconnect(err);
		}
	else
		{
		// If this is a new device that has become active send a progress notification.
		if(!aNewConnection.NewConnectedDeviceProgressSent())
			{
			SendProgress(EPanAgtConnectedNewDevice, KErrNone);
			aNewConnection.SetNewConnectedDeviceProgressSent(ETrue);
			}
			
		// Check to see if an uplink access progress notification needs to be sent
		SendUplinkAccessRequiredProgressNotification();
		}
	iDeviceSelector->Reset();
	}

void CPanAgent::DeviceRoleChangeFailed(CPanRemoteDeviceStateMachine& aDevice)
/**
Indicates that the given device failed to change role
@pre The device has been in the active state and is now renegotiating roles
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));

	TRAPD(err, iState->DeviceRoleChangeFailedL(aDevice));	
	if(err)
		{
		LOG1(_L("CPanAgent: DeviceRoleChangeFailed left with %d"), err);
		RequestDisconnect(err);
		}	
	}

void CPanAgent::DeviceDisconnected(CPanRemoteDeviceStateMachine& aDevice)
/**
Indicates that the remote device has disconnected, and should be deleted from the device array
*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iState, PanAgentPanic(ENoState));

#ifdef __FLOG_ACTIVE
	TBuf<KMaxBtAddrSize> devaddr;
	aDevice.DevAddr().GetReadable(devaddr, KNullDesC, KBtAddrSeparator, KNullDesC);
	LOG1(_L("Remote device %S disconnected"), &devaddr);
#endif
		
	if(aDevice.NewConnectedDeviceProgressSent())
		{
		SendProgress(EPanAgtDisconnectedDevice, KErrNone);
		aDevice.SetNewConnectedDeviceProgressSent(EFalse);
		}

	// Check if any remaining active devices require the uplink.
	SendUplinkAccessRequiredProgressNotification();

	TRAPD(err, iState->DeviceDisconnectedL(aDevice));	// if this leaves, then shutdown everything
	
	//check if there are more devices we want to connect to
	TInt errNewConnection = KErrNone;
	while (iDeviceSelector->HasMoreDevices())
		{
		LOG(_L("More devices available, trying to establish connection"));
		TRAP(errNewConnection, iDeviceSelector->CreateNextConnectionL());
		if (errNewConnection == KErrNone ) 
			{
			LOG(_L("Created new connection to next available device"));
			return;
			}
		else
			{
			LOG1(_L("Failed to create new connection, error=%d"), errNewConnection);
			}
		}
	
	if( err != KErrNone || errNewConnection != KErrNone )
		{
		LOG(_L("...no more active devices, shutting down"));
		RequestDisconnect(err);
		}
	}
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Return pointer to the main BNEP connection manager class
*/
const Messages::TNodeId& CPanAgent::BnepConnectionManager()
	{
	LOG_FUNC
	return iBnepConnectionManager;
	}

/**
Restarts the incoming connection listener when it has been waiting
for a response from BNEP
*/
void CPanAgent::RestartIncomingConnectionListener(TInt aError)
    {
	LOG_FUNC
    iIncomingConnectionListener->RestartIncomingConnectionListener(aError);
    }

void CPanAgent::SetRetryConnect(CPanRemoteDeviceStateMachine& /*aDevice*/) 
	{
	LOG_FUNC
    CPanAgtStateBase* nextState = NULL;
    TInt err = KErrNone;
	TRAP(err, nextState = CPanAgtStateIdle::NewL(*this));
	if(err != KErrNone) 
		{
		if(iShutdownState)
			{
			SetState(*iShutdownState);
			}
		return;
		}
	
	CPanAgtStatePaused* pausedState = NULL;
	TRAP(err,
	    CleanupStack::PushL(nextState);
	    pausedState = CPanAgtStatePaused::NewL(*this, *nextState);
	    CleanupStack::Pop();
	    );

    if (err != KErrNone)
        {
		if(iShutdownState)
			{
			SetState(*iShutdownState);
			}
		return;
        }

	delete iState;
	SetState(*pausedState);

    // Only if all the above worked do we send the message to BNEP
	TPanMessage::TSetRetryConnect msg;
    RClientInterface::OpenPostMessageClose(Id(),BnepConnectionManager(),msg);
	}
	
#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
Return pointer to the main BNEP connection manager class
*/
MBnepConnectionManager& CPanAgent::BnepConnectionManager()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iBnepConnectionManager, PanAgentPanic(ENullBnepConnectionManager));
	
	return(*iBnepConnectionManager);
	}

void CPanAgent::SetRetryConnect(CPanRemoteDeviceStateMachine& /*aDevice*/) 
	{
	LOG_FUNC
	iBnepConnectionManager->SetRetryConnect();
	
	CPanAgtStateBase* nextState = NULL;
	TRAPD(err, nextState = CPanAgtStateIdle::NewL(*this));
	if(err == KErrNone) 
		{
		delete iState;
		SetState(*nextState);
		}
	else
		{
		if(iShutdownState)
			{
			SetState(*iShutdownState);
			}
		}
	}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY



void CPanAgent::CloseExistingConnectionWithUplinkAccess()
	{
	LOG_FUNC
	TInt count = iRemoteDevices.Count();
	for(TInt i=0;i<count;i++)
		{
		if(iRemoteDevices[i]->UplinkAccessAllowed())
			{
			// Close this connection.
			iRemoteDevices[i]->Disconnect();
			break;
			}
		}
	}

void CPanAgent::GetExistingConnections(TPanConnectionList& aPanConnectionList)
	{
	LOG_FUNC
	TBool localRoleSet = EFalse;
	TInt count = iRemoteDevices.Count();
	for(TInt i=0;i<count;i++)
		{
		if(iRemoteDevices[i]->GetState() == EActive)
			{
			if(!localRoleSet)
				{
				// Set the local role.
				aPanConnectionList.SetCurrentLocalRole(TUUID(iRemoteDevices[i]->LocalRole()));
				localRoleSet = ETrue;
				}
				
			TBTDevAddr deviceAddr = iRemoteDevices[i]->DevAddr();
			TPanConnection requestedConn(deviceAddr, iRemoteDevices[i]->UplinkAccessAllowed());

			TRAPD(err, aPanConnectionList.AddRemoteConnectionL(requestedConn));
			if(err != KErrNone)
				{
				LOG1(_L("CPanAgent::GetExistingConnections - Failed to add existing connection [%d]"), i);
				break;
				}
			}
		}
	
	if(!localRoleSet)
		{
		aPanConnectionList.SetCurrentLocalRole(TUUID(0));
		}
	}
	
	
TBool CPanAgent::WorthTrying(const TPanDeviceWorthTryingRolesList& aRemoteWorthTryingRoles) const
	{
	LOG_FUNC
	TBool ret = ETrue;	
	if(iFixedRemoteRole != EPanRoleUnknown) // check for fixed remote role
		{
		ret = aRemoteWorthTryingRoles.IsWorthTryingARole(iFixedRemoteRole); // check if it is worth trying
		}
	return ret;
	}


#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
MPanLinkControlBase& CPanAgent::BnepConnectionController()
	{
	LOG_FUNC
	return *this;
	}


TAny* CPanAgent::MplcbGetInterface(const TUid & aInterface) 
	{
	LOG_FUNC
	if (aInterface ==KUidPanLinkControlV1) 
		{
		return PanLinkControlInterface();
		}
	return NULL;
	}
	
MPanLinkControl* CPanAgent::PanLinkControlInterface()
	{
	LOG_FUNC
	return this;
	}

TInt CPanAgent::MplcRequestSniff(const TBTDevAddr & aAddr)
	{
	LOG_FUNC
	CPanRemoteDeviceStateMachine* dev=FindRemoteDeviceByAddress(aAddr);
	if (dev) 
		{
		return dev->PhysicalLinkAdapter().ActivateSniffRequester();
		}
	else 
		{
		return KErrNotFound;
		}
	}

TInt CPanAgent::MplcRequestActive(const TBTDevAddr & aAddr)
	{
	LOG_FUNC
	CPanRemoteDeviceStateMachine* dev=FindRemoteDeviceByAddress(aAddr);
	if (dev) 
		{
		return dev->PhysicalLinkAdapter().ActivateActiveRequester();
		}
	else 
		{
		return KErrNotFound;
		}

	}
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	
//
// PAN agent helper methods
//

CPanRemoteDeviceStateMachine* CPanAgent::FindRemoteDeviceByAddress(const TBTDevAddr& remoteDeviceAddress)
/**
Find the remote device with the specified address
@note Simple linear search (the array size is small) - easier than constructing a temporary to use
with RPointerArray::Find()
*/
	{
	LOG_FUNC
	for(TInt i=0; i < iRemoteDevices.Count(); ++i)
		{
		if(iRemoteDevices[i]->DevAddr() == remoteDeviceAddress)
			{
			return (iRemoteDevices[i]);
			}
		}
	return NULL;
	}
	
void CPanAgent::CheckDeviceIsNotAlreadyAttachedL(const TBTDevAddr& aDevAddr)
/**
Ensure the remote device is not already a member of the PAN network
(PAN profile only allows one BNEP connection between devices)
*/
	{
	LOG_FUNC

	if(FindRemoteDeviceByAddress(aDevAddr))
		{
		LOG(_L("CPanAgent: ...it is - disconnecting it"));

		User::Leave(KErrAlreadyExists);
		}
		
	LOG(_L("CPanAgent: ...it isn't - creating new connection"));
	}

void CPanAgent::RequestDisconnect(TInt aError)
/**
Request from role state machine to disconnect
@note Any code in this class that wants to trigger a disconnect should call this method.
@note This is designed to be called only from the role state machine - states can trigger
a shutdown by leaving from within their state
@param aError The error code that occurred, or KErrNone if the shutdown state does not know
what the error is
*/
	{
	LOG_FUNC
	LOG1(_L("CPanAgent: request disconnect from state with error %d"), aError);
	
	if(aError!=KErrNone) // shutdown state may not know what the error is, in which case it will
						 // pass KErrNone, and we should preserve the previous value of iError
		{
		iError = aError;
		}

	if(iNifmanConnected) // once connected, we need to disconnect when there's an error
		{
		iInitiateDisconnectCallback->CallBack();
		}
	else // we haven't actually brought the connection up properly yet, so rather than issuing 
		 //a Stop() we should just pass an error code back to nifman in ConnectComplete()
		{
		__ASSERT_DEBUG(iError!=KErrNone, PanAgentPanic(ESomeoneLeftWithoutGivingAReason)); // there's a problem, but no-one's set an error code to say what it is!
		}
	}

//
// Agent extended management interface methods
//

TInt CPanAgent::GetInterfaceType(TConnectionType& aConnectionType)
/**
Return the type of this interface
*/
	{
	LOG_FUNC
	aConnectionType = EConnectionBTPAN;
	return KErrNone;
	}

TInt CPanAgent::EnumerateSubConnections(TUint& aCount)
/**
Return the number of subconnections
@note Connections to a device are not treated as subconnections, as subconnections are reserved
currently reserved to refer to QoS channels - since BT PAN profile does not support QoS at the 
moment, there are no subconnections to expose
*/
	{
	LOG_FUNC
	aCount = 1; // the "whole connection" subconnection
	return KErrNone;
	}
	
TInt CPanAgent::GetSubConnectionInfo(TUint /*aIndex*/, TDes8& /*aSubConnectionInfo*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}
	
TInt CPanAgent::GetSubConnectionInfo(TDes8& /*aSubConnectionInfo*/)
	{
	LOG_FUNC
	return KErrNotSupported;
	}

TBool CPanAgent::AllowIncoming() const
	{
	LOG_FUNC
	return iAllowIncoming;
	}
	
void CPanAgent::SetAllowIncoming(TBool aAllowIncoming)
	{
	LOG_FUNC
	iAllowIncoming = aAllowIncoming;
	}

TBool CPanAgent::NapEnabled() const
	{
	LOG_FUNC
	return iNapEnabled;
	}

void CPanAgent::SetNapEnabled(TBool aNapEnabled)	
	{
	LOG_FUNC
	iNapEnabled = aNapEnabled;
	}

TInt CPanAgent::TryToCreateNewPanConnection()
	{
	LOG_FUNC
	if (iPanCoexistenceConnectionController)
		{
		return iPanCoexistenceConnectionController->TryToCreateNewPanConnection();
		}
	else
		{
		// Make sure the release build works fine if iPanCoexistenceConnectionController is null
		LOG(_L("CPanAgent::TryToCreateNewPanConnection - iPanCoexistenceConnectionController is NULL"));
		return KErrNone;
		}
	}

void CPanAgent::HandleAllPanConnectionsClosed()
	{
	LOG_FUNC
	if (iPanCoexistenceConnectionController)
		{
		iPanCoexistenceConnectionController->HandleAllPanConnectionsClosed();
		}
	else
		{
		// Log it when iPanCoexistenceConnectionController is null
		LOG(_L("CPanAgent::HandleAllPanConnectionsClosed - iPanCoexistenceConnectionController is NULL"));
		}
	}

TBool CPanAgent::IsNewPanConnectionAllowed() const
	{
	LOG_FUNC
	TBool rValue = EFalse;
	
	if(iPanCoexistenceConnectionController)
		{
		TIPBearerCoexistenceStatus status = iPanCoexistenceConnectionController->ReadStatus();
		if(status == EBTPanIsActive || status == ENoneIsActive)
			{
			__ASSERT_DEBUG(status != EBTPanIsActive, PanAgentPanic(EImproperSituationToSetPropertyToBTPanIsActive));
			rValue = ETrue;
			}
		}
	else
		{
		LOG(_L("CPanAgent::IsNewPanConnectionAllowed - iPanCoexistenceConnectionController is NULL"));
		//Make sure the release build works fine if iPanCoexistenceConnectionController is null
		rValue = ETrue;
		}

	return rValue;
	}

void CPanAgent::SendUplinkAccessRequiredProgressNotification()
	{
	LOG_FUNC
	// Check if any active devices require the uplink.
	TBool uplinkRequired = EFalse;
	TInt count = iRemoteDevices.Count();
	for(TInt i=0;i<count;i++)
		{
		if(iRemoteDevices[i]->GetState() == EActive && iRemoteDevices[i]->UplinkAccessAllowed())
			{
			uplinkRequired = ETrue;
			break;
			}
		}

	if(iUplinkRequiredProgressSent && !uplinkRequired)
		{
		// The uplink is no longer required send a progress notification
		iUplinkRequiredProgressSent = EFalse;
		SendProgress(EPanAgtUplinkNotRequired, KErrNone);
		}
	else if(!iUplinkRequiredProgressSent && uplinkRequired)
		{
		// The uplink is now required, send a progress notification
		iUplinkRequiredProgressSent = ETrue;
		SendProgress(EPanAgtUplinkRequired, KErrNone);
		}
	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
*/
void CPanAgent::ReceivedL(const Messages::TRuntimeCtxId& /* aSender */, const Messages::TNodeId& /* aRecipient */, Messages::TSignatureBase& aCFMessage)
    {
	LOG_FUNC
	if (aCFMessage.MessageId().Realm() == TPanMessage::ERealmId)
    	{
        switch (aCFMessage.MessageId().MessageId())
            {
            case TPanMessage::TSetRetryConnectComplete::EId:
                {
                // We would expect that iState should always be the paused state when
                // we get here. We assert it in debug builds in case something we
                // haven't accounted for changes it unexpectedly
                __ASSERT_DEBUG(iState->iStateNumber == CPanAgtStateBase::EPanAgtStatePaused, PanAgentPanic(EPanAgtUnexpectedStateChange));
                if (iState->iStateNumber == CPanAgtStateBase::EPanAgtStatePaused)
                    {
                    static_cast<CPanAgtStatePaused*>(iState)->TransitionToNextState();
                    }
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





	
//
// State base class implementations
//

void CPanAgtStateBase::DeviceActiveL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedDeviceActiveReceivedByState);	
	}

void CPanAgtStateBase::DeviceRoleChangeFailedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedDeviceRoleChangeFailedReceivedByState);
	}
			
void CPanAgtStateBase::DeviceDisconnectedL(CPanRemoteDeviceStateMachine& /*aDevice*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedDeviceDisconnectedReceivedByState);
	}

void CPanAgtStateBase::StateSendRoleNotification()
/**
Base class implementation - will be called but should need to do nothing, just log
overridden for states EPanAgtStateURole and EPanAgtStateGnRole	
*/
	{
	LOG_FUNC
	LOG1(_L("PanAgtSB - CPanAgtStateBase::StateSendRoleNotification() called for state %d"),iStateNumber);
	}
	
void CPanAgtStateBase::IncomingConnectionFromPeerL(TBluetoothPanRole /*aLocalRole*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedIncomingConnectionFromPeerReceivedByState);
	}

void CPanAgtStateBase::RoleChangeRequestFromPeerL(TBluetoothPanRole /*aLocalRole*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedRoleChangeRequestFromPeerReceivedByState);	
	}

void CPanAgtStateBase::InitiateOutgoingConnectionL(TBluetoothPanRole& /*aLocalRole*/, TBluetoothPanRole& /*aRemoteRole*/, TPanDeviceWorthTryingRolesList& /*aWorthTryingRemoteRoles*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedInitiateOutgoingConnectionReceivedByState);	
	}

void CPanAgtStateBase::PerformLocalRoleChangeRequestL(TBluetoothPanRole& /*aLocalRole*/, TBluetoothPanRole& /*aRemoteRole*/)
/**
Base class implementation - should never be called
*/
	{
	LOG_FUNC
	PanicInState(EUnexpectedPerformLocalRoleChangeRequestReceivedByState);	
	}

CPanAgtStateBase::CPanAgtStateBase(MPanAgtStateMachineNotify& aStateMachine, TPanAgtStates aStateNumber) :
	iStateMachine(aStateMachine), iStateNumber(aStateNumber)
/**
When implementing a new state that derives from this base class, ensure that
it provides a unique number to identify the state from other possible
states.  @see PanAgent::TPanAgtStates.
*/
	{
	LOG_FUNC

	}

void CPanAgtStateBase::StartNegotiationWithFirstPendingDevice()
/**
Check for any idle devices in the device array, and trigger negotiation on the first one; if there are no idle devices this method returns without doing anything.
@note Device negotiation is performed in the order that devices are held in the array - new devices should be inserted at the end of the array
*/
	{
	LOG_FUNC
	
	// find first pending device in array and call ReadyForRoleRequest
	for(TInt i=0;i<StateMachine().Devices().Count();++i)
		{
		CPanRemoteDeviceStateMachine& device = *StateMachine().Devices()[i];

		if(device.GetState() == EPendingNegotiation)
			{
			LOG(_L("PanAgtSB - ...device with negotiation pending found - calling ReadyForRoleRequest"));
			device.ReadyForRoleRequest();
			return;
			}
		}
	LOG(_L("PanAgtSB - ...no pending devices found"));
	}

TInt CPanAgtStateBase::IsRoleRequestValid(TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole) const
/**
Check the role request is legal according to the PAN profile spec
@param aRequestedLocalRole The requested local role
@param aRequestedRemoteRole The requested remote role
@return KErrNone if role request is OK, KErrArgument if the role combination was illegal
@note Not a bug as such, but just a convenient way to get doxygen to record that this method embeds knowledge 
of legal PAN role combinations, and allows EPanRoleUnknown as a valid local role.  Later code must decide on 
a concrete local role if this is the case.
*/
	{
	LOG_FUNC
	// check for invalid role combinations
	if((aRequestedLocalRole==EPanRoleGn || aRequestedLocalRole==EPanRoleNap) && (aRequestedRemoteRole==EPanRoleGn || aRequestedRemoteRole==EPanRoleNap))
		{
		LOG2(_L("PanAgtSB - CheckRoleRequest: invalid role combination received (%x, %x)"), aRequestedLocalRole, aRequestedRemoteRole);
		return KErrArgument;		
		}
	else
		{
		return KErrNone;
		}
	}

TInt CPanAgtStateBase::SelectRemoteRoleBasedOnLocalRole(TBluetoothPanRole aSelectedLocalRole, TBluetoothPanRole& aRemoteRole, TPanDeviceWorthTryingRolesList& aWorthTryingRemoteRoles)
/**
Given a selected local role, check what role has been proposed for the remote device and alter it accordingly
@note This handles setting the remote role if it is fixed in commdb
@param aSelectedLocalRole The local role that has been selected by the role state machine
@param aRemoteRole On entry, the role that was proposed by the remote device state machine. On return, the 
remote role that will be requested
@return The same error codes and meanings as MRemoteDeviceNotify::RoleRequest()
@note Embeds knowledge of valid PAN profile role combinations
*/
	{
	LOG_FUNC
	TBluetoothPanRole fixedRemoteRole = StateMachine().FixedRemoteRole();
	
	// Check to see if there's a fixed remote role first...
	if(fixedRemoteRole != EPanRoleUnknown) // check for fixed remote role
		{
		LOG1(_L("PanAgtSB - SelectRemoteRole: ...remote role is fixed as %x..."), StateMachine().FixedRemoteRole());
		
		if(aWorthTryingRemoteRoles.IsWorthTryingARole(fixedRemoteRole) || fixedRemoteRole == EPanRoleU)
			{
			// if the remote device doesn't support the role (and it's not U role, which isn't 
			// necessarily advertised in SDP (see PAN profile spec v1.0, section 8.1.3)
			// then return an error
			aRemoteRole = fixedRemoteRole;
			aWorthTryingRemoteRoles.SetWorthTrying(fixedRemoteRole, EFalse);
			}
		else
			{
			LOG(_L("PanAgtSB - SelectRemoteRole: ...fixed remote role is not supported! (not included in SDP)"));
			return KErrInvalidOrUnacceptableRoleCombination;
			}
		}
	else
		{
		// ...otherwise use dynamic role selection
		switch(aSelectedLocalRole)
			{
			case EPanRoleU:
				{
				LOG(_L("PanAgtSB - CheckAndAdjustRemoteRole: no SDP query to use for remote role selection, making a guess and trying U role - there's not much else we can do"));
				// see PAN profile spec v1.0, section 8.1.3 for reasoning
				
				/* now if we don't know what they do support
				   and the other side has refused us, or we didn't ask
				   then we might ask for something else
				 */
				// lets decide what we'll try this time
				if (aWorthTryingRemoteRoles.IsWorthTryingU())
					{
					aRemoteRole = EPanRoleU;
					}
				else if (aWorthTryingRemoteRoles.IsWorthTryingGn())
					{
					aRemoteRole = EPanRoleGn;
					}
				else if (aWorthTryingRemoteRoles.IsWorthTryingNap())
					{
					aRemoteRole = EPanRoleNap;
					}

				aWorthTryingRemoteRoles.SetWorthTrying(aRemoteRole, EFalse);
				}
			 	break;	// RVCT detects this as dead code - if you change the logic above, *beware*
						// you may need this!
			case EPanRoleGn:
			case EPanRoleNap:
				aRemoteRole = EPanRoleU;
				break;

			default:
				PanicInState(EPanAgentRoleStateMachineSelectedUnknownAsLocalRole);
			};
		}

	return KErrNone;
	}

void CPanAgtStateBase::DoDeviceDisconnectL(CPanRemoteDeviceStateMachine& aDevice)
/**
Delete the disconnected device from the array, and check if it was the last device. If it was, and we're
not allowing incoming connections, then shutdown.
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(FindDeviceInArray(aDevice)>=0, PanicInState(EDeviceDisconnectedThatWasNotInArray));

	TInt index = FindDeviceInArray(aDevice);
	if(index>=0) // ie. the device is in the array
		{
		LOG(_L("RoleState: <[base]> - device disconnected"));
		delete StateMachine().Devices()[index];
		StateMachine().Devices().Remove(index);
		}
		
	if (StateMachine().Devices().Count()==0)
		{
		StateMachine().HandleAllPanConnectionsClosed();
		
		// see if this was the last device, and if we're not listening for incoming connections
		if (!(StateMachine().AllowIncoming()) && !(StateMachine().IsPromptingUserForDeviceSelection()))
			{
			User::Leave(KErrAllDevicesDisconnected);
			}
		
		}
	}

TInt CPanAgtStateBase::NegotiatingDeviceCount() const
/**
The number of devices in the device array in negotiating state
@return The number of devices in the device array in negotiating state
*/
	{
	LOG_FUNC
	return(DeviceCountInState(EPerformingNegotiation));	
	}


TInt CPanAgtStateBase::ActiveDeviceCount() const
/**
The number of devices in the device array in active state
@return The number of devices in the device array in active state
*/
	{
	LOG_FUNC
	return(DeviceCountInState(EActive));	
	}

MPanAgtStateMachineNotify& CPanAgtStateBase::StateMachine()
/**
Return a handle to the state machine
*/
	{
	LOG_FUNC
	return(iStateMachine);
	}
	
const MPanAgtStateMachineNotify& CPanAgtStateBase::StateMachine() const
/**
Return a const handle to the state machine
*/
	{
	LOG_FUNC
	return(iStateMachine);
	}

TInt CPanAgtStateBase::FindDeviceInArray(const CPanRemoteDeviceStateMachine& aDevice) const
/**
Find a given device in the array
@param aRequestingConnection The connection handle to find
@return Either the position of the device in the array, or KErrNotFound
*/
	{
	LOG_FUNC
	for(TInt i=0; i<StateMachine().Devices().Count(); i++)
		{
		if(StateMachine().Devices()[i] == &aDevice)
			{
			return i;	
			}
		}
	return KErrNotFound;	
	}

TInt CPanAgtStateBase::DeviceCountInState(TRemoteDeviceState aState) const
/**
The number of devices in the given state
@param aState Devices in this state should be counted
@return The number of devices in the given state
*/
	{
	LOG_FUNC
#ifdef _DEBUG
	DumpDeviceArray();
#endif
	TInt devCountInState = 0;
	
	for(TInt i=0; i<StateMachine().Devices().Count(); i++)
		{
		if((StateMachine().Devices()[i]->GetState()) == aState)
			{
			++devCountInState;
			}
		}
		
	return(devCountInState);
	}

void CPanAgtStateBase::LockDeviceAsPiconetMasterL()
/**
Set link policy to stop other devices changing the baseband role, once we are in the master role
@note The ASSERT to ensure that we are in the master role is performed in the remote device state machines
in debug mode only
*/
	{
	LOG_FUNC
	__ASSERT_DEBUG(NegotiatingDeviceCount()==0, PanicInState(ETryingToLockBasebandRolesWhenDevicesAreNegotiating));

	for(TInt i=0; i < StateMachine().Devices().Count(); ++i)
		{
		// lock any device that is active into it's current baseband role (which should be master)
		// can't lock other devices, because in hardware that supports scatternetting, they
		// might still be in slave role, because we haven't gone through the role negotiation
		// process with them yet, which is where we do baseband role switches
		if(StateMachine().Devices()[i]->GetState() == EActive)
			{
			User::LeaveIfError(StateMachine().Devices()[i]->DisallowRoleSwitch());
			}
		}
	}
	
TInt CPanAgtStateBase::ReleasePiconetMasterLock()
/**
Set link policy to allow other devices to change the baseband role
*/
	{
	LOG_FUNC
	TInt count;
	TInt err;
	for(count=0; count < StateMachine().Devices().Count(); ++count)
		{
		err = StateMachine().Devices()[count]->AllowRoleSwitch();
		if(!err)
			{
			continue;
			}
		else	// go back through the devices resetting the link policy to lock us as master
			{
			for(--count; count >= 0; --count)	// decrement before starting so we only reset role switching for the ones that we have previously set 
				{
				if(StateMachine().Devices()[count]->GetState() == EActive)
					{
					// ignore errors - if we don't then the only other choice is to shut down everything!
					StateMachine().Devices()[count]->DisallowRoleSwitch();
					}
				}
			}
		break;
		}
	return KErrNone;
	}

void CPanAgtStateBase::StartListenerL()
/**
Work out which local roles we support and register the appropriate records
*/
	{
	LOG_FUNC
	
	if(StateMachine().AllowIncoming())
		{
		__ASSERT_DEBUG(StateMachine().FixedLocalRole() != EPanRoleU, PanAgentPanic(EFixedLocalRoleUForListeningIap));

		// Register U role if fixed local role is not specified.
		if(StateMachine().FixedLocalRole() == EPanRoleUnknown)
			{
			StateMachine().PanLocalSdpRegistrar().RegisterLocalSdpRecordL(EPanRoleU, StateMachine().IapId());
			}
		
		// Register either GN or NAP	
		if(StateMachine().NapEnabled())
			{				
			StateMachine().PanLocalSdpRegistrar().RegisterLocalSdpRecordL(EPanRoleNap, StateMachine().IapId());
			}
		else
			{
			StateMachine().PanLocalSdpRegistrar().RegisterLocalSdpRecordL(EPanRoleGn, StateMachine().IapId());
			}

		User::LeaveIfError(StateMachine().StartIncomingConnectionListener());
		}
	}

void CPanAgtStateBase::StopListener()
	{
	LOG(_L("StopListener - remove local SDP registation and close listener."));
		
	//check that listerner is started
	if(StateMachine().IsIncomingConnectionListenerEnabled())
		{
		__ASSERT_DEBUG(StateMachine().FixedLocalRole() != EPanRoleU, PanAgentPanic(EFixedLocalRoleUForListeningIap));

		// Unregister U role if fixed local role is not specified.
		if(StateMachine().FixedLocalRole() == EPanRoleUnknown)
			{
			StateMachine().PanLocalSdpRegistrar().UnregisterLocalSdpRecord(EPanRoleU);
			}
		
		// Unregister either GN or NAP	
		if(StateMachine().NapEnabled())
			{				
			StateMachine().PanLocalSdpRegistrar().UnregisterLocalSdpRecord(EPanRoleNap);
			}
		else
			{
			StateMachine().PanLocalSdpRegistrar().UnregisterLocalSdpRecord(EPanRoleGn);
			}
		
		StateMachine().CloseIncomingConnectionListener();
		}
	}

void CPanAgtStateBase::Connect()
	{
	LOG_FUNC
	iServiceStartedCallback->CallBack();
	}

void CPanAgtStateBase::CancelConnect()
	{
	LOG_FUNC
	iServiceStartedCallback->Cancel();
	}

void CPanAgtStateBase::ServiceStarted()
	{
	LOG_FUNC

	StateMachine().SendProgress(EPanAgtInitialising, KErrNone);
	iConnectCompleteCallback->CallBack(); // Queued here before ServiceStarted as ServiceStarted can delete iConnectCompleteCallback, causing a null pointer exception 
	StateMachine().ServiceStarted(); 
	
	}

TInt CPanAgtStateBase::ServiceStartedCb(TAny* aThisPtr)
/**
Connection Callback static function 
*/
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aThisPtr, PanAgentPanic(ENullTAnyPointer));

	CPanAgtStateBase* self = static_cast<CPanAgtStateBase*>(aThisPtr);
	self->ServiceStarted();
	return KErrNone;
	}

TInt CPanAgtStateBase::ConnectCompleteCb(TAny* aThisPtr)
/**
Second callback used during connection creation
*/
	{
	LOG_STATIC_FUNC
	__ASSERT_DEBUG(aThisPtr, PanAgentPanic(ENullTAnyPointer));

	CPanAgtStateBase* self = static_cast<CPanAgtStateBase*>(aThisPtr);
	self->StateMachine().ConnectComplete();
	return KErrNone;
	}
	
void CPanAgtStateBase::BaseConstructL()
	{
	LOG_FUNC
	iConnectCompleteCallback = new (ELeave) CAsyncCallBack(KPanAgtAoPriority);	
	TCallBack connectCompleteCallback(ConnectCompleteCb, this);
	iConnectCompleteCallback->Set(connectCompleteCallback);
	
	iServiceStartedCallback = new (ELeave) CAsyncCallBack(KPanAgtAoPriority);
	TCallBack serviceStartedCallback(ServiceStartedCb, this);
	iServiceStartedCallback->Set(serviceStartedCallback);
	}
	
CPanAgtStateBase::~CPanAgtStateBase()
	{
	LOG_FUNC
	iServiceStartedCallback->Cancel();
	delete iServiceStartedCallback;
	iServiceStartedCallback = NULL;
	
	iConnectCompleteCallback->Cancel();
	delete iConnectCompleteCallback;
	iConnectCompleteCallback = NULL;
	}

void CPanAgtStateBase::CancelReconnect()
	{
	LOG_FUNC
	//do nothing
	}
/**
Calls the appropriate panic function to encode the panic
code with the current state identifier.
@param aPanic The panic code that the state is panicking with.
*/
void CPanAgtStateBase::PanicInState(TPanAgentPanic aPanic) const
	{
	LOG_FUNC
	PanAgentPanic(aPanic, iStateNumber);
	}

#ifdef _DEBUG
void CPanAgtStateBase::DumpDeviceArray() const
/**
Dump the device array to the logs
*/	
	{
	LOG_FUNC
	const RPointerArray<CPanRemoteDeviceStateMachine>& deviceArray(StateMachine().Devices());																					
	
	for(TInt i=0; i<deviceArray.Count(); i++)
		{
		LOG2(_L("Device: %x, state: %d"), deviceArray[i], deviceArray[i]->GetState());
		}
	}
#endif // _DEBUG	


