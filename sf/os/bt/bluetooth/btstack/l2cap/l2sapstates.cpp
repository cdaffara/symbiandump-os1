// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implement the L2CAP sap state classes.
// These are CL2CAPSAPStateFactory, TL2CAPSAPState (abstract) and its derived
// classes
// Together, these classes and the SAP implement the State pattern
// (GOF).  The states themselves are implemented using the Flyweight
// pattern.  Each state is a Flyweight object, and CL2CAPSAPStateFactory
// is manager of these objects.  As a result of being a flyweight, no
// state object may have state that can't be shared between all
// possible users of the state (ie no per-SAP state)
// 
//

#include <bluetooth/logger.h>

#include <bt_sock.h>

#include "l2sapstates.h"

#include "L2CapSDUQueue.h"
#include "L2CapDataController.h"
#include "l2capSAPSignalHandler.h"

#include "l2sap.h"
#include "l2cap.h"

#include "l2util.h"

#ifdef _DEBUG
#include "L2CapDebugControlInterface.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

/*************************************************************************/
//
// TL2CAPSAPState Implementation
//

TL2CAPSAPState::TL2CAPSAPState(CL2CAPSAPStateFactory& aFactory)
: iFactory(aFactory)
	{
	LOG_FUNC
	}

void TL2CAPSAPState::PanicInState(TL2CAPPanic aPanic) const
	{
	LOG_FUNC
	Panic(aPanic, iFactory.StateIndex(this));
	}

#ifdef _DEBUG
void TL2CAPSAPState::DebugPanicInState(TL2CAPPanic aPanic) const
#else
void TL2CAPSAPState::DebugPanicInState(TL2CAPPanic /*aPanic*/) const
#endif
	{
	LOG_FUNC
	#ifdef _DEBUG
	PanicInState(aPanic);
	#endif
	}

void TL2CAPSAPState::StartCalledWhileDisconnected(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// This function is intended to be called (under some circumstances when a Start()
	// has been received. This handles the case where in between the Start() call and
	// the construction of the cloned SAP on which the Start is being made, the link
	// which prompted the cloning has disconnected.  Therefore, if we are a cloned sap
	// and have been "coupled" with an client side socket we need to inform it of the
	// disconnection.
	if(aSAP.ListeningSAP())
		{
		if(aSAP.Socket())
			{
			if(aSAP.SocketErrorCode() == KErrNone)
				{
				aSAP.Socket()->Disconnect();
				}
			else
				{
				aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
				}
			}
		// We also need to take ourselves off the listening SAP's 'clone' queue, just 
		// as we do in the fair weather case when Start is called in state 'Accepting'.
		aSAP.DetachFromListeningSAP();
		}
	}

// DEFAULT Implementations
// Socket Initiated.
void TL2CAPSAPState::Start(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}

TInt TL2CAPSAPState::Ioctl(CL2CAPConnectionSAP& /*aSAP*/, TUint /*aLevel*/, TUint /*aName*/, TDes8* /*aOption*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	return KErrSAPUnexpectedEvent;
	}


void TL2CAPSAPState::CancelIoctl(CL2CAPConnectionSAP& /*aSAP*/, TUint /*aLevel*/, TUint /*aName*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}

void TL2CAPSAPState::IoctlComplete(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErr*/, TUint /*aLevel*/, TUint /*aName*/, TDesC8* /*aBuf*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}
	
void TL2CAPSAPState::ActiveOpen(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}

TInt TL2CAPSAPState::PassiveOpen(CL2CAPConnectionSAP& /*aSAP*/, TUint /*aQueSize*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	return KErrSAPUnexpectedEvent;
	}

void TL2CAPSAPState::Shutdown(CL2CAPConnectionSAP& /* aSAP */) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}

void TL2CAPSAPState::FastShutdown(CL2CAPConnectionSAP& /* aSAP */) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}

TInt TL2CAPSAPState::Send(CL2CAPConnectionSAP& /*aSAP*/, RMBufChain& /*aData*/, TUint /*aFlag*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	return KErrNotReady; // Indicate that no data has been sent.
	}

TInt TL2CAPSAPState::Read(CL2CAPConnectionSAP& /*aSAP*/, RMBufChain& /*aData*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	return KErrNotReady;
	}
	
void TL2CAPSAPState::Bound(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSocketEvent);
	}
	
// Events called from the SAP Signal Handler.
void TL2CAPSAPState::ChannelConfigured(CL2CAPConnectionSAP& /*aSAP*/,
	                                   CL2CapChannelConfig& /*aConfig*/,
                                       CL2CAPMux& /*aMuxer*/,
                                       TL2CAPPort /*aLocalPort*/,
                                       TL2CAPPort /*aRemotePort*/)
	{
	LOG_FUNC
	DebugPanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}

void TL2CAPSAPState::ReconfiguringChannel(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	DebugPanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}

void TL2CAPSAPState::ChannelClosed(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}

void TL2CAPSAPState::CloseOutgoingSDUQueue(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}	

void TL2CAPSAPState::SignalHandlerError(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}	

void TL2CAPSAPState::LinkUp(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}	

void TL2CAPSAPState::ChannelOpened(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}	

// Events called from the Data Controller.
void TL2CAPSAPState::NewData(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	}

TInt TL2CAPSAPState::NewDataAsyncCallBack(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	//Need to return something to stop error in compiler
	return(KErrNone);
	}

void TL2CAPSAPState::CanSend(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	}

void TL2CAPSAPState::SDUQueueClosed(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	}

void TL2CAPSAPState::DataPlaneError(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	}

// Events from the security manager.
void TL2CAPSAPState::AccessRequestComplete(CL2CAPConnectionSAP& /*aSignalHandler*/, TInt /*aResult*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSecurityManagerEvent);
	}


// State Transition Actions.
void TL2CAPSAPState::Enter(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	}
	
void TL2CAPSAPState::Exit(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	}
	
// Helper method.
void TL2CAPSAPState::NotifySocketClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Enter the closed state.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	
	// Check if this is owned by the socket yet.  If it is the socket needs
	// to be notified, and is responsible for deleting this SAP.
	if(aSAP.Socket())
		{
		switch(aSAP.ShutdownReceived())
			{
			case CL2CAPConnectionSAP::ESAPShutdownNormal:
				aSAP.Socket()->CanClose();
				break;

			case CL2CAPConnectionSAP::ESAPShutdownNone:
				if(aSAP.SocketErrorCode() == KErrNone)
					{
					aSAP.Socket()->Disconnect();
					}
				else
					{
					// An error has occurred, enter the error state.
					aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EError));
					aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
					}
				break;

			case CL2CAPConnectionSAP::ESAPShutdownImmediate:
				break;
				
			default:
				PanicInState(EL2CAPInvalidSAPShutdownType);
				break;	
			};
		}
	else
		{
		if(!aSAP.IsAcceptPending())
			{
			// This is not owned by a socket - delete it.
			aSAP.DetachFromListeningSAP();
			aSAP.DeleteSAP();
			}
		}
	}

/*************************************************************************/
//
// TL2CAPSAPStateClosed Implementation
//
TL2CAPSAPStateClosed::TL2CAPSAPStateClosed(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}

// Events called from the SAP.
void TL2CAPSAPStateClosed::Start(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// If a passive connection was established and then disconnected by the peer
	// the Start notification will be received in this state.
	// This is handled in the Closed state rather than in the Bound state since
	// we're not bound from the stack's point of view in this situation.
	StartCalledWhileDisconnected(aSAP);
	}

void TL2CAPSAPStateClosed::ActiveOpen(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EActiveLinkPending));

	// If there is a Service set for CodMan, register it.
	aSAP.RegisterCodService();

	TInt rerr = aSAP.Protocol().MuxController().AttachActiveSignalHandler(aSAP.SignalHandler(), aSAP.RemoteDev());
	if(rerr != KErrNone)
		{
		// Set up an error condition to indicate that the connection
		// has failed.
		aSAP.SetSocketErrorCode(KErrCouldNotConnect);
		aSAP.SetSocketErrorAction(MSocketNotify::EErrorConnect);
		
		NotifySocketClosed(aSAP);	
		}
	}
	
void TL2CAPSAPStateClosed::SDUQueueClosed(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Nothing needs to be done here.  FastShutdown scenario.
	}
	
// Events called from the SAP Signal Handler.
void TL2CAPSAPStateClosed::ChannelClosed(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Ignore in this state.  This will be due to a fast shutdown.
	}

void TL2CAPSAPStateClosed::SignalHandlerError(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/) const
	{
	LOG_FUNC
	// Ignore in this state.  This will be due to a fast shutdown.
	}	

void TL2CAPSAPStateClosed::Enter(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.iNewDataToNotify = 0;	
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	}

void TL2CAPSAPStateClosed::FastShutdown(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Shutdown is permitted by ESOCK on an open (not connected) socket.
	// Therefore this must be handled in the closed or bound state.
	// No action required here.
	}

void TL2CAPSAPStateClosed::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Shutdown is permitted by ESOCK on an open (not connected) socket.
	// Therefore this must be handled in the closed or bound state.
	aSAP.Socket()->CanClose();
	}
	
void TL2CAPSAPStateClosed::Bound(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EBound));
	}

TInt TL2CAPSAPStateClosed::NewDataAsyncCallBack(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	//drop
	return EFalse;
	}

/*************************************************************************/
//
// TL2CAPSAPStateChannelPendingBase Implementation
//
TL2CAPSAPStateChannelPendingBase::TL2CAPSAPStateChannelPendingBase(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}
 
// Events called from the SAP Signal Handler.
void TL2CAPSAPStateChannelPendingBase::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Store the error information.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);
	
	NotifySocketClosed(aSAP);	
	}

void TL2CAPSAPStateChannelPendingBase::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Set up an error condition to indicate that the connection
	// has failed.
	aSAP.SetSocketErrorCode(KErrCouldNotConnect);
	aSAP.SetSocketErrorAction(MSocketNotify::EErrorConnect);
	
	NotifySocketClosed(aSAP);	
	}

// Events called from the SAP.
void TL2CAPSAPStateChannelPendingBase::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Enter the disconnecting state and disconnect the Mux.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));
	aSAP.SignalHandler().CloseChannelRequest();
	}

void TL2CAPSAPStateChannelPendingBase::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Enter the closed state and disconnect the Mux.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	aSAP.SignalHandler().CloseChannelRequest();
	}
		
/*************************************************************************/
//
// TL2CAPSAPStatePassiveLinkPending Implementation
//
TL2CAPSAPStatePassiveLinkPending::TL2CAPSAPStatePassiveLinkPending(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateChannelPendingBase(aFactory)
 	{
	LOG_FUNC
 	}

void TL2CAPSAPStatePassiveLinkPending::LinkUp(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Forward the Connect Request event to the signalling state machine.
	aSAP.SignalHandler().ConnectRequestReceived();
	
	// Start security checks.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EPassiveAccessRequestPending));
	aSAP.StartAccessRequest(*(aSAP.ListeningSAP()));
	}

/*************************************************************************/
//
// TL2CAPSAPStateActiveLinkPending Implementation
//
TL2CAPSAPStateActiveLinkPending::TL2CAPSAPStateActiveLinkPending(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateChannelPendingBase(aFactory)
 	{
	LOG_FUNC
 	}

void TL2CAPSAPStateActiveLinkPending::LinkUp(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// This is the first call to StartAccessRequest() for v2.1 clients.
	// For outgoing requests, we perform authentication both before the L2CAP
	// connection is started, and immediately after (for compatibility with
	// pre 2.1 clients). Here, we just set the required MITM settings; this
	// has no effect on pre-2.1 clients but this will succeed for 2.1 clients
	// and the subsequent call is effectively a null-operation.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EActiveSecMode4AccessRequestPending));
	aSAP.StartAccessRequest(aSAP, ETrue); // indicate that this is outgiong security mode 4
	}

/*************************************************************************/
//
// TL2CAPSAPStateActiveSecMode4AccessRequestPending Implementation
//
TL2CAPSAPStateActiveSecMode4AccessRequestPending::TL2CAPSAPStateActiveSecMode4AccessRequestPending(CL2CAPSAPStateFactory& aFactory)
: TL2CAPSAPStateChannelPendingBase(aFactory)
	{
	LOG_FUNC
	}

// Events called from the SAP.
void TL2CAPSAPStateActiveSecMode4AccessRequestPending::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::FastShutdown(aSAP);
	}
	
void TL2CAPSAPStateActiveSecMode4AccessRequestPending::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::Shutdown(aSAP);
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStateActiveSecMode4AccessRequestPending::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::ChannelClosed(aSAP);
	}

void TL2CAPSAPStateActiveSecMode4AccessRequestPending::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::SignalHandlerError(aSAP, aErrorCode, aErrorAction);
	}
	
void TL2CAPSAPStateActiveSecMode4AccessRequestPending::AccessRequestComplete(CL2CAPConnectionSAP& aSAP, TInt aResult) const
	{
	LOG_FUNC
	// Check the result from the security manager.
	switch(aResult)
		{
		case EBTSecManAccessGranted:
			// Inform the signalling state machine that the connect can be established.
			aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EActiveChannelRequestPending));
			aSAP.SignalHandler().OpenChannelRequest();
			break;
			
		case EBTSecManAccessDenied:
		default:  // Handle any error
			// Store the error information.
			aSAP.SetSocketErrorCode(KErrCouldNotConnect);
			aSAP.SetSocketErrorAction(MSocketNotify::EErrorConnect);
			
			// Call the helper method to move to the closed state and 
			// let the peer know there is a security problem
			aSAP.SignalHandler().CloseChannelRequest();
			break;
		};
	}

/*************************************************************************/
//
// TL2CAPSAPStateActiveChannelRequestPending Implementation
//
TL2CAPSAPStateActiveChannelRequestPending::TL2CAPSAPStateActiveChannelRequestPending(CL2CAPSAPStateFactory& aFactory)
: TL2CAPSAPStateChannelPendingBase(aFactory)
	{
	LOG_FUNC
	}
	
void TL2CAPSAPStateActiveChannelRequestPending::ChannelOpened(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// A Channel is open but not configured.  Perform security mode 2 security before 
	// configuring the channel.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EActiveSecMode2AccessRequestPending));
	
	// This is the second call to StartAccessRequest(), for pre-v2.1 clients.
	// For v2.1 clients, the authentication with MITM settings will have
	// already occurred, and so this call effectively does nothing.
	aSAP.StartAccessRequest(aSAP);
	}

/*************************************************************************/
//
// TL2CAPSAPStatePassiveAccessRequestPending Implementation
//
TL2CAPSAPStatePassiveAccessRequestPending::TL2CAPSAPStatePassiveAccessRequestPending(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateChannelPendingBase(aFactory)
 	{
	LOG_FUNC
 	}

// Events called from the SAP.
void TL2CAPSAPStatePassiveAccessRequestPending::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::FastShutdown(aSAP);
	}
	
void TL2CAPSAPStatePassiveAccessRequestPending::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::Shutdown(aSAP);
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStatePassiveAccessRequestPending::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::ChannelClosed(aSAP);
	}

void TL2CAPSAPStatePassiveAccessRequestPending::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::SignalHandlerError(aSAP, aErrorCode, aErrorAction);
	}

// Events from the security manager.
void TL2CAPSAPStatePassiveAccessRequestPending::AccessRequestComplete(CL2CAPConnectionSAP& aSAP, TInt aResult) const
	{
	LOG_FUNC
	// Check the result from the security manager.
	switch(aResult)
		{
		case EBTSecManAccessGranted:
			// Inform the signalling state machine that the connect can be established.
			aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EPendingOpen));
			aSAP.SignalHandler().OpenChannelRequest();
			break;

		case EBTSecManAccessDenied:
		default:  // Handle any error
			// Store the error information.
			aSAP.SetSocketErrorCode(KErrCouldNotConnect);
			aSAP.SetSocketErrorAction(MSocketNotify::EErrorConnect);

			// Call the helper method to move to the closed state and 
			// let the peer know there is a security problem
			aSAP.SignalHandler().CloseChannelRequest();
			break;
		};		
	}

/*************************************************************************/
//
// TL2CAPSAPStateActiveSecMode2AccessRequestPending Implementation
//
TL2CAPSAPStateActiveSecMode2AccessRequestPending::TL2CAPSAPStateActiveSecMode2AccessRequestPending(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateChannelPendingBase(aFactory)
 	{
	LOG_FUNC
 	}

// Events called from the SAP.
void TL2CAPSAPStateActiveSecMode2AccessRequestPending::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::FastShutdown(aSAP);
	}
	
void TL2CAPSAPStateActiveSecMode2AccessRequestPending::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::Shutdown(aSAP);
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStateActiveSecMode2AccessRequestPending::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::ChannelClosed(aSAP);
	}

void TL2CAPSAPStateActiveSecMode2AccessRequestPending::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	aSAP.CancelAccessRequest();
	TL2CAPSAPStateChannelPendingBase::SignalHandlerError(aSAP, aErrorCode, aErrorAction);
	}

// Events from the security manager.
void TL2CAPSAPStateActiveSecMode2AccessRequestPending::AccessRequestComplete(CL2CAPConnectionSAP& aSAP, TInt aResult) const
	{
	LOG_FUNC
	// Check the result from the security manager.
	switch(aResult)
		{
		case EBTSecManAccessGranted:
			// Inform the signalling state machine that configuration can be started.
			aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EPendingOpen));
			aSAP.SignalHandler().ConfigureChannelRequest();
			break;

		case EBTSecManAccessDenied:
		default:  // Handle any error
			// Store the error information.
			aSAP.SetSocketErrorCode(KErrCouldNotConnect);
			aSAP.SetSocketErrorAction(MSocketNotify::EErrorConnect);

			// Enter the disconnecting state and disconnect the Mux.
			aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));
			aSAP.SignalHandler().CloseChannelRequest();
			break;
		};		
	}
	
/*************************************************************************/
//
// TL2CAPSAPStatePendingOpen Implementation
//
TL2CAPSAPStatePendingOpen::TL2CAPSAPStatePendingOpen(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateChannelPendingBase(aFactory)
 	{
	LOG_FUNC
 	}


void TL2CAPSAPStatePendingOpen::ChannelConfigured(CL2CAPConnectionSAP& aSAP,
	                                              CL2CapChannelConfig& aConfig,
                                                  CL2CAPMux& aMuxer,
                                                  TL2CAPPort aLocalPort,
                                                  TL2CAPPort aRemotePort)
	{
	LOG_FUNC
	TInt rerr = aSAP.CreateDataPlane(aConfig, aMuxer, aLocalPort, aRemotePort);
	if(rerr == KErrNone)
		{
		// This changes the state.
		aSAP.SocketConnectComplete();
		}
	else
		{
		// Failed to create the data plane.  Close the connection and error the
		// socket.
		DataPlaneError(aSAP, rerr, MSocketNotify::TOperationBitmasks(MSocketNotify::EErrorSend | MSocketNotify::EErrorRecv));
		}
	}  

// Events called from the Data Controller.
void TL2CAPSAPStatePendingOpen::DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Store the error condition, and disconnect the signalling.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);

	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));
	aSAP.SignalHandler().CloseChannelRequest();
	}
			

/*************************************************************************/
//
// TL2CAPSAPStateListening Implementation
//
TL2CAPSAPStateListening::TL2CAPSAPStateListening(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}

// Events called from the SAP.
void TL2CAPSAPStateListening::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// The listening sap is shutting down.
	aSAP.SignalHandler().CloseChannelRequest();
	}

void TL2CAPSAPStateListening::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	aSAP.SignalHandler().CloseChannelRequest();
	}
	
// State Transition Actions.
void TL2CAPSAPStateListening::Enter(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.Protocol().MuxController().AttachListeningSignalHandler(aSAP.SignalHandler());		
	}

void TL2CAPSAPStateListening::Exit(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	aSAP.Protocol().DecrementListeners();
	aSAP.DeleteAllClones();
	
	// Remove the signal handler from the list of listening SH.
	aSAP.SignalHandler().iLink.Deque();
	
	// Check to see if the stack should be brought down
	aSAP.Protocol().TryToClose();
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStateListening::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Call the helper method to move to the closed state and signal to the
	// socket that close is complete.
	NotifySocketClosed(aSAP);	
	}


/*************************************************************************/
//
// TL2CAPSAPStateAccepting Implementation
//
TL2CAPSAPStateAccepting::TL2CAPSAPStateAccepting(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateOpen(aFactory)
 	{
	LOG_FUNC
 	}
 	
void TL2CAPSAPStateAccepting::ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.PauseDataPlane();
	}

// Events called from the Data Controller.
void TL2CAPSAPStateAccepting::NewData(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.iNewDataToNotify++;
	}

void TL2CAPSAPStateAccepting::Start(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EOpen));
	aSAP.DetachFromListeningSAP();
	aSAP.StartNewDataAsyncCallBack();
	}

TInt TL2CAPSAPStateAccepting::NewDataAsyncCallBack(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	// Keep the compiler happy.
	return KErrNone;
	}
	
void TL2CAPSAPStateAccepting::CanSend(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedDataControllerEvent);
	}

// State Transition Actions.
void TL2CAPSAPStateAccepting::Exit(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	
	aSAP.RegisterCodService();
	}
	
/*************************************************************************/
//
// TL2CAPSAPStateAwaitingInitialData Implementation
//
TL2CAPSAPStateAwaitingInitialData::TL2CAPSAPStateAwaitingInitialData(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPStateOpen(aFactory)
 	{
	LOG_FUNC
 	}
 	
void TL2CAPSAPStateAwaitingInitialData::ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.PauseDataPlane();
	}
 	
 // Events called from the Data Controller.
void TL2CAPSAPStateAwaitingInitialData::NewData(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SignalHandler().ChannelConfigured();
	
	aSAP.iNewDataToNotify++;
	aSAP.StartNewDataAsyncCallBack();
	
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EOpen));
	}

/*************************************************************************/
//
// TL2CAPSAPStateOpen Implementation
//
TL2CAPSAPStateOpen::TL2CAPSAPStateOpen(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}


void TL2CAPSAPStateOpen::ReconfiguringChannel(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.PauseDataPlane();
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EAwaitingInitialData));
	}

void TL2CAPSAPStateOpen::ChannelConfigured(CL2CAPConnectionSAP& aSAP,
	                                       CL2CapChannelConfig& aConfig,
                                           CL2CAPMux& /*aMuxer*/,
                                           TL2CAPPort /*aLocalPort*/,
                                           TL2CAPPort /*aRemotePort*/)
	{
	LOG_FUNC
	//This will be called after a successful reconfigure
	const TL2CapFecNegotiator& fec = aConfig.FecNegotiator();
	TL2CapDataControllerConfig* dataConfig = new TL2CapDataControllerConfig(fec.DataControllerConfig());

	if(dataConfig)
		{
		aSAP.DataQueue().ResumeSDUQueue(dataConfig,
									 	aConfig.OutgoingFlushTimeout().Negotiated().FlushTimeoutDuration(),
		                   				fec.OutgoingMaximumPDUSize(),
                       				    aConfig.OutgoingMTU().Negotiated().MTU(),
                       				    aConfig.IncomingMTU().Negotiated().MTU());

		// Adjust the optimal PDU size based on the new configuration values
		TPckgBuf<TInt> buf;	
		TInt err = aSAP.GetOption(KSolBtACL, ELMOutboundACLSize, buf);

		TInt optimalPduSize = HL2CapPDU::GetPDUOrFragmentSize(aConfig.OutgoingMTU().Negotiated().MTU(), fec.OutgoingMaximumPDUSize(), (err == KErrNone) ? buf() : 0, aSAP.DataQueue().IsBasicDataVersion());
		aSAP.DataQueue().SetOptimalPDUSize(optimalPduSize);

		// If this reconfiguration was initiated via an IOCTL complete it.
		aSAP.TryToCompleteConfigurationIoctl(KErrNone);
		}
	else
		{
		// If this reconfiguration was initiated via an IOCTL complete it.
		aSAP.TryToCompleteConfigurationIoctl(KErrNoMemory);
		DataPlaneError(aSAP, KErrNoMemory, MSocketNotify::TOperationBitmasks(MSocketNotify::EErrorSend | MSocketNotify::EErrorRecv));
		}
	}	

// Events called from the SAP.
TInt TL2CAPSAPStateOpen::Send(CL2CAPConnectionSAP& aSAP, RMBufChain& aData, TUint /*aFlag*/) const
	{
	LOG_FUNC
	TInt rerr = aSAP.DataQueue().Write(aData);
	if(rerr != KErrNone && rerr != KErrNotReady)
		{
		// Need to signal an error for the send operation.
		aSAP.Socket()->Error(rerr, MSocketNotify::EErrorSend);
		}
	return rerr;
	}
	
TInt TL2CAPSAPStateOpen::Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const
	{
	LOG_FUNC
	// The return value is either an error or the number of datagrams read.
	TInt rValue = KErrNone;

	aSAP.DataQueue().Read(aData);
	if(aData.Length() > aSAP.DataQueue().MaxIncomingMTU())
		{
		// This is larger then the negotiated MRU.  Drop the SDU.
		aData.Free();
		rValue = KErrOverflow;
		}
	else
		{
		rValue = (aData.Length() > 0);
		}
	return rValue;
	}

void TL2CAPSAPStateOpen::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));
	
	aSAP.DataQueue().CloseSDUQueue(CL2CapSDUQueue::EImmediatelyCloseIncomingQueue,
		                           CL2CapSDUQueue::EDrainOutgoingQueue);
	}

void TL2CAPSAPStateOpen::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// This needs to be shutdown within this stack frame.
	// Move into the Closed state (Note. This transition de-activates 
	// park mode).  Attempt to send a Disconnect Requests, and delete 
	// the data plane. 
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	aSAP.SignalHandler().CloseChannelRequest();
	aSAP.SDUQueueClosed();	
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStateOpen::CloseOutgoingSDUQueue(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));

	// Close the Outgoing SDU queue.	
	aSAP.DataQueue().CloseSDUQueue(CL2CapSDUQueue::EDrainIncomingQueue,
		                           CL2CapSDUQueue::EImmediatelyCloseOutgoingQueue);
	}	

void TL2CAPSAPStateOpen::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));

	// Close the Outgoing SDU queue.	
	aSAP.DataQueue().CloseSDUQueue(CL2CapSDUQueue::EDrainIncomingQueue,
		                           CL2CapSDUQueue::EImmediatelyCloseOutgoingQueue);
	}

void TL2CAPSAPStateOpen::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));

	CL2CapSDUQueue::TIncomingSDUQueueCloseAction icAct = (aErrorAction & MSocketNotify::EErrorRecv) ?
	                                                     CL2CapSDUQueue::EImmediatelyCloseIncomingQueue :
	                                                     CL2CapSDUQueue::EDrainIncomingQueue;

	CL2CapSDUQueue::TOutgoingSDUQueueCloseAction ogAct = (aErrorAction & MSocketNotify::EErrorSend) ?
	                                                     CL2CapSDUQueue::EImmediatelyCloseOutgoingQueue :
	                                                     CL2CapSDUQueue::EDrainOutgoingQueue;
	                                                   
	// Close the queue. Drain the queues according to the
	// error action.
	aSAP.DataQueue().CloseSDUQueue(icAct, ogAct);

	// Store the error information.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);
	}


// Events called from the Data Controller.
void TL2CAPSAPStateOpen::NewData(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.iNewDataToNotify++;
	aSAP.StartNewDataAsyncCallBack();
	}
	
TInt TL2CAPSAPStateOpen::NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Datagram i/f -- pass up no. of packets.
	TUint16 newData = aSAP.iNewDataToNotify;
	if(newData)
		{
		aSAP.iNewDataToNotify = 0;
		aSAP.Socket()->NewData(newData);
		}
	return EFalse;
	}
	
void TL2CAPSAPStateOpen::CanSend(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.Socket()->CanSend();
	}

// Events called from the Data Controller.
void TL2CAPSAPStateOpen::DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Store the error condition, and disconnect the signalling.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);

	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EDisconnecting));

	// Close the data plane.
	aSAP.SDUQueueClosed();
	aSAP.SignalHandler().CloseChannelRequest();
	}
	

/*************************************************************************/
//
// TL2CAPSAPStateDisconnecting Implementation
//
TL2CAPSAPStateDisconnecting::TL2CAPSAPStateDisconnecting(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}
 
void TL2CAPSAPStateDisconnecting::Start(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// We do the same here as in the EClosed state, as under exceptional
	// circumstances a slight race will result in the Start() coming in
	// this state.
	StartCalledWhileDisconnected(aSAP);
	}

// Events called from the SAP.
TInt TL2CAPSAPStateDisconnecting::Send(CL2CAPConnectionSAP& /*aSAP*/, RMBufChain& /*aData*/, TUint /*aFlag*/) const
	{
	LOG_FUNC
	return KErrNotReady; // Indicate that no data has been sent.
	}

TInt TL2CAPSAPStateDisconnecting::Read(CL2CAPConnectionSAP& aSAP, RMBufChain& aData) const
	{
	LOG_FUNC
	// The return value is either an error or the number of datagrams read.
	TInt rValue = KErrNone;

	// Check if the queue is closed.
	if(!aSAP.IsSDUQueueClosed())
		{
		aSAP.DataQueue().Read(aData);
		if(aData.Length() > aSAP.DataQueue().MaxIncomingMTU())
			{
			// This is larger then the negotiated MRU.  Drop the SDU.
			aData.Free();
			rValue = KErrOverflow;
			}
		else
			{
			rValue = (aData.Length() > 0);	
			}
		}
	return rValue;
	}

void TL2CAPSAPStateDisconnecting::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	LOG(_L("L2CAP disconnecting state: shutdown"));
	
	// If the queue is still active, indicate that the incoming
	// queue no longer needs to be drained.
	if(!aSAP.IsSDUQueueClosed())
		{
		LOG(_L(" -- need to close SDU queue"));

		aSAP.DataQueue().CloseSDUQueue(CL2CapSDUQueue::EImmediatelyCloseIncomingQueue,
		                               CL2CapSDUQueue::EDrainOutgoingQueue);
		}
	}

void TL2CAPSAPStateDisconnecting::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	LOG(_L("L2CAP disconnecting state: fast shutdown"));

	// This needs to be shutdown within this stack frame.
	// Delete the data plane, attempt to send a Disconnect Requests
	// and move into the Closed state.	
	aSAP.SignalHandler().CloseChannelRequest();

	// This transition removes the park mode override.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	aSAP.SDUQueueClosed();	
	}

// Events called from the SAP Signal Handler.
void TL2CAPSAPStateDisconnecting::ChannelClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// If the data plane is closed there is nothing more to be done.
	if(aSAP.IsSDUQueueClosed())
		{
		// Call the helper method to move to the closed state and signal to the
		// socket that close is complete.
		NotifySocketClosed(aSAP);	
		}
	}

void TL2CAPSAPStateDisconnecting::SignalHandlerError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Store the error information.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);

	// The signalling has errored so a Disconnect Complete is no longer
	// expected.
	if(aSAP.IsSDUQueueClosed())
		{
		// Call the helper method to move to the closed state and signal to the
		// socket that close is complete.
		NotifySocketClosed(aSAP);	
		}
	else
		{
		// The SDU queue is still active.  Ensure the correct queues are getting drained.
		CL2CapSDUQueue::TIncomingSDUQueueCloseAction icAct = (aErrorAction & MSocketNotify::EErrorRecv) ?
		                                                     CL2CapSDUQueue::EImmediatelyCloseIncomingQueue :
		                                                     CL2CapSDUQueue::EDrainIncomingQueue;

		CL2CapSDUQueue::TOutgoingSDUQueueCloseAction ogAct = (aErrorAction & MSocketNotify::EErrorSend) ?
		                                                     CL2CapSDUQueue::EImmediatelyCloseOutgoingQueue :
		                                                     CL2CapSDUQueue::EDrainOutgoingQueue;
		                                                   
		// Close the queue. Drain the queues according to the
		// error action.
		aSAP.DataQueue().CloseSDUQueue(icAct, ogAct);
		}
	}

void TL2CAPSAPStateDisconnecting::CloseOutgoingSDUQueue(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Close the Outgoing SDU queue.	
	aSAP.DataQueue().CloseSDUQueue(CL2CapSDUQueue::EImmediatelyCloseIncomingQueue,
		                           CL2CapSDUQueue::EImmediatelyCloseOutgoingQueue);
	}	

// Events called from the Data Controller.
void TL2CAPSAPStateDisconnecting::SDUQueueClosed(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// The SDU queue has been disconnected.  Check if Disconnect signalling 
	// is complete.
	if(aSAP.SignalHandler().IsChannelClosed())
		{
		// Call the helper method to move to the closed state and signal to the
		// socket that close is complete.
		NotifySocketClosed(aSAP);	
		}
	else
		{
		// Signal disconnect to the signal handler.
		aSAP.SignalHandler().CloseChannelRequest();
		}
	}


void TL2CAPSAPStateDisconnecting::DataPlaneError(CL2CAPConnectionSAP& aSAP, TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Store the error condition, and disconnect the signalling.
	aSAP.SetSocketErrorCode(aErrorCode);
	aSAP.SetSocketErrorAction(aErrorAction);

	// Close the data plane.
	aSAP.SDUQueueClosed();
	}

void TL2CAPSAPStateDisconnecting::NewData(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Data received in this state can be silently dropped.
	}

TInt TL2CAPSAPStateDisconnecting::NewDataAsyncCallBack(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	//Note we may have data received before moving into the 
	//disconnect state that we haven't yet informed ESock about
	
	//Datagram i/f -- pass up no. of packets
	TUint16 newData = aSAP.iNewDataToNotify;
	if(newData)
		{
		aSAP.iNewDataToNotify = 0;
		aSAP.Socket()->NewData(newData);
		}
	return EFalse; 
	}

void TL2CAPSAPStateDisconnecting::CanSend(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Check that the SDU queue is still open, and a Shutdown
	// has not been received.
	if(!aSAP.IsSDUQueueClosed() && aSAP.ShutdownReceived() == CL2CAPConnectionSAP::ESAPShutdownNone)
		{
		aSAP.Socket()->CanSend();
		}
	}

void TL2CAPSAPStateDisconnecting::ChannelConfigured(CL2CAPConnectionSAP& /*aSAP*/,
	                                   CL2CapChannelConfig& /*aConfig*/,
                                       CL2CAPMux& /*aMuxer*/,
                                       TL2CAPPort /*aLocalPort*/,
                                       TL2CAPPort /*aRemotePort*/)
	{
	LOG_FUNC
	// Drop this event as the SAP is closing.  The SAP signal handler
	// is still open to handle the exchange with the remote until
	// the connection is closed.
	}

void TL2CAPSAPStateDisconnecting::ReconfiguringChannel(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Drop this event as the SAP is closing.  The SAP signal handler
	// is still open to handle the exchange with the remote until
	// the connection is closed.
	}

/*************************************************************************/
//
// TL2CAPSAPStateError Implementation
//
TL2CAPSAPStateError::TL2CAPSAPStateError(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}

// Events called from the SAP.
void TL2CAPSAPStateError::Start(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	}
	
TInt TL2CAPSAPStateError::Ioctl(CL2CAPConnectionSAP& aSAP, TUint /*aLevel*/, TUint /*aName*/, TDes8* /*aOption*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	return KErrNotReady;
	}
	
void TL2CAPSAPStateError::CancelIoctl(CL2CAPConnectionSAP& aSAP, TUint /*aLevel*/, TUint /*aName*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	}
	
void TL2CAPSAPStateError::IoctlComplete(CL2CAPConnectionSAP& aSAP, TInt /*aErr*/, TUint /*aLevel*/, TUint /*aName*/, TDesC8* /*aBuf*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	}
	

void TL2CAPSAPStateError::ActiveOpen(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	}
	
TInt TL2CAPSAPStateError::PassiveOpen(CL2CAPConnectionSAP& aSAP, TUint /*aQueSize*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	return KErrNotReady;	
	}
	
void TL2CAPSAPStateError::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// The socket wishes to close the SAP, signal can close.
	aSAP.Socket()->CanClose();
	}
	
void TL2CAPSAPStateError::FastShutdown(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// The SAP is about to be deleted by the socket. No action required in this state.
	}
	

TInt TL2CAPSAPStateError::Send(CL2CAPConnectionSAP& aSAP, RMBufChain& /*aData*/, TUint /*aFlag*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	return KErrNotReady; // Indicate that no data has been sent.
	}
	
TInt TL2CAPSAPStateError::Read(CL2CAPConnectionSAP& aSAP, RMBufChain& /*aData*/) const
	{
	LOG_FUNC
	// Error the socket to error/complete the outstanding request.
	aSAP.Socket()->Error(aSAP.SocketErrorCode(), aSAP.SocketErrorAction());
	return KErrNotReady;
	}
	
// Events called from the SAP Signal Handler.
void TL2CAPSAPStateError::ChannelClosed(CL2CAPConnectionSAP& /*aSAP*/) const
	{
	LOG_FUNC
	// Already in the error state so no need to transition.
	// Consume the event.
	}
	
void TL2CAPSAPStateError::SignalHandlerError(CL2CAPConnectionSAP& /*aSAP*/, TInt /*aErrorCode*/, MSocketNotify::TOperationBitmasks /*aErrorAction*/) const
    {
    LOG_FUNC
    // Already in the error state so no need to transition.
    // Consume the event.
    }   

// State Transition Actions.
void TL2CAPSAPStateError::Enter(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Set the error action to error all requests from the socket.
	aSAP.SetSocketErrorAction(MSocketNotify::EErrorAllOperations);
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	}

/*************************************************************************/
//
// TL2CAPSAPStateBound Implementation
//
TL2CAPSAPStateBound::TL2CAPSAPStateBound(CL2CAPSAPStateFactory& aFactory)
 : TL2CAPSAPState(aFactory)
 	{
	LOG_FUNC
 	}
 	
TInt TL2CAPSAPStateBound::PassiveOpen(CL2CAPConnectionSAP& aSAP, TUint aQueSize) const
	{
	LOG_FUNC
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EListening));

	// would be nice to use Listening::Enter but return path of TInt is lost
	TInt rerr = aSAP.Protocol().IncrementListeners();

	aSAP.RegisterCodService();	// See if there is a Service set for CodMan

	if(rerr == KErrNone)
		{
		aSAP.iMaxAcceptingQCount = static_cast<TUint8>(aQueSize);
		__ASSERT_DEBUG(aSAP.iClones.Count() == 0, PanicInState(EL2CAPUnexpectedSocketEvent));
		}
	return rerr;
	}

// State Transition Actions.
void TL2CAPSAPStateBound::Enter(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	aSAP.Protocol().MuxController().AttachBoundSignalHandler(aSAP.SignalHandler());
	}
	
void TL2CAPSAPStateBound::Exit(CL2CAPConnectionSAP& aSAP) const	
	{
	LOG_FUNC
	// Remove the signal handler from the list of bound SH.
	aSAP.SignalHandler().iLink.Deque();	
	
	// Check to see if the stack needs to be brought down
	aSAP.Protocol().TryToClose();
	}

void TL2CAPSAPStateBound::FastShutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Shutdown is permitted by ESOCK on an open (not connected) socket.
	// Therefore this must be handled in the closed or bound state.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	}

void TL2CAPSAPStateBound::Shutdown(CL2CAPConnectionSAP& aSAP) const
	{
	LOG_FUNC
	// Shutdown is permitted by ESOCK on an open (not connected) socket.
	// Therefore this must be handled in the closed or bound state.
	aSAP.SetState(iFactory.GetState(CL2CAPSAPStateFactory::EClosed));
	aSAP.Socket()->CanClose();
	}


/*************************************************************************/
//
// CL2CAPSAPStateFactory Implementation
//
CL2CAPSAPStateFactory::CL2CAPSAPStateFactory()
	{
	LOG_FUNC
	}

CL2CAPSAPStateFactory::~CL2CAPSAPStateFactory()
	{
	LOG_FUNC
	// Destroy all the state objects
	iStates.DeleteAll();
	}
	
CL2CAPSAPStateFactory* CL2CAPSAPStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CL2CAPSAPStateFactory* factory= new (ELeave) CL2CAPSAPStateFactory();
	CleanupStack::PushL(factory);

	// Create all the new states
	factory->iStates[EClosed]								= new (ELeave) TL2CAPSAPStateClosed(*factory);
	factory->iStates[EPassiveLinkPending]					= new (ELeave) TL2CAPSAPStatePassiveLinkPending(*factory);
	factory->iStates[EActiveLinkPending]					= new (ELeave) TL2CAPSAPStateActiveLinkPending(*factory);
	factory->iStates[EPassiveAccessRequestPending]			= new (ELeave) TL2CAPSAPStatePassiveAccessRequestPending(*factory);
	factory->iStates[EActiveSecMode2AccessRequestPending]	= new (ELeave) TL2CAPSAPStateActiveSecMode2AccessRequestPending(*factory);
	factory->iStates[EPendingOpen]							= new (ELeave) TL2CAPSAPStatePendingOpen(*factory);
	factory->iStates[EListening]							= new (ELeave) TL2CAPSAPStateListening(*factory);
	factory->iStates[EAccepting]							= new (ELeave) TL2CAPSAPStateAccepting(*factory);
	factory->iStates[EAwaitingInitialData]					= new (ELeave) TL2CAPSAPStateAwaitingInitialData(*factory);
	factory->iStates[EOpen]									= new (ELeave) TL2CAPSAPStateOpen(*factory);
	factory->iStates[EDisconnecting]						= new (ELeave) TL2CAPSAPStateDisconnecting(*factory);
	factory->iStates[EError]								= new (ELeave) TL2CAPSAPStateError(*factory);
	factory->iStates[EBound]								= new (ELeave) TL2CAPSAPStateBound(*factory);
	factory->iStates[EActiveSecMode4AccessRequestPending]	= new (ELeave) TL2CAPSAPStateActiveSecMode4AccessRequestPending(*factory);
	factory->iStates[EActiveChannelRequestPending]			= new (ELeave) TL2CAPSAPStateActiveChannelRequestPending(*factory);

	CleanupStack::Pop();
	return factory;
	}

TL2CAPSAPState& CL2CAPSAPStateFactory::GetState(const TStates aState) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EMaxState, Panic(EL2CAPSAPMuxerStateOutOfBounds));
	return *iStates[aState];
	}

TInt CL2CAPSAPStateFactory::StateIndex(const TL2CAPSAPState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < EMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KL2UnknownState;
	}

