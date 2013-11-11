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

// Implements the signalling state machine for an L2Cap Channel
#include <bluetooth/logger.h>

#include "l2capSAPSignalHandler.h"
#include "l2capSigStates.h"
#include "l2capSigPacketConnection.h"
#include "l2capSigPacketConfigure.h"
#include "l2capSigPacketDisconnection.h"
#include "l2capSigPacketEcho.h"
#include "l2capSigPacketCommandReject.h"

#include "l2capSigStates.h"
#include "l2signalmgr.h"

#include "l2capSAPSignalHandler.h"
#include "l2cap.h"
#include "l2sap.h"

#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

/*************************************************************************/
//
// TL2CAPSigState Implementation
// 

TL2CAPSigState::TL2CAPSigState(const CL2CAPSignalStateFactory& aFactory):
	iFactory(aFactory)
	{
	LOG_FUNC
	}

// Events from the SAP
void TL2CAPSigState::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// This is a standard action for most states.
	// Disconnect received from the SAP.  Return to the
	// closed state.
	aSignalHandler.HandleLinkError();
	aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		
	// Signal back to the SAP that disconnection is complete.
	aSignalHandler.SAP()->ChannelClosed();
	}

void TL2CAPSigState::OpenChannel(CL2CapSAPSignalHandler& /*aSignalHandler*/) const	
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPEvent);
	}

void TL2CAPSigState::ConnectRequestReceived(CL2CapSAPSignalHandler& /*aSignalHandler*/) const	
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPEvent);
	}

void TL2CAPSigState::ConfigureChannel(CL2CapSAPSignalHandler& /*aSignalHandler*/) const	
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPEvent);
	}

// L2CAP commands received from the peer.
void TL2CAPSigState::ConnectResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/,
						 			 HConnectionResponse* /*aConnectionResponse*/) const
	{
	LOG_FUNC
	LOG1(_L("Unhandled Connect Response, state = 0x%08x"), this);
	}


void TL2CAPSigState::ConfigRequest(CL2CapSAPSignalHandler& /*aSignalHandler*/,
										  HConfigureRequest* /*aConfigRequest*/) const
	{
	LOG_FUNC
	LOG1(_L("Unhandled Config Req, state = 0x%08x"), this);
	}

void TL2CAPSigState::ConfigResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/,
 	                					   HConfigureResponse* /*aConfigResponse*/) const
	{
	LOG_FUNC
	LOG1(_L("Unhandled Config Response, state = 0x%08x"), this);
	}
	
void TL2CAPSigState::DisconnectRequest(CL2CapSAPSignalHandler& /*aSignalHandler*/, 
                                              TUint8 /*aId*/) const
	{
	LOG_FUNC
	LOG1(_L("Unhandled Disconnect Request, state = 0x%08x"), this);
	}

void TL2CAPSigState::DisconnectResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	LOG1(_L("Unhandled Disconnect Response, state = 0x%08x"), this);
	}

// Events from the Mux	
void TL2CAPSigState::Error(CL2CapSAPSignalHandler& aSignalHandler,
	                       TInt aErrorCode, 
	                       MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// An error has occurred in the Mux or the signal handler.  This means no
	// more signalling can be done.  Move to the closed state.  Signal link down
	// to clear any pending commands and stop waiting for outstanding responses.
	// Inform the SAP of the error.
	aSignalHandler.HandleLinkError();
	aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));

	// This will delete the signal handler if the SAP has detached.
	aSignalHandler.SignalHandlerErrorD(aErrorCode, aErrorAction);
	}

TInt TL2CAPSigState::UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const
	{
	LOG_FUNC
	// The default action is to update the values.  Different actions are taken
	// if configuration is in progress or the channel is open.
	TBool reconfigRequired; //  this only matters when the channel is already Open, can ignore it here
	return aSignalHandler.ChannelConfig().UpdateConfigAPIChange(aAPIConfig, reconfigRequired);
	}

TInt TL2CAPSigState::GetNegotiatedChannelMode(const CL2CapSAPSignalHandler& /*aSignalHandler*/, TL2CapChannelMode& /*aMode*/) const
	{
	return KErrNotReady;
	}

// Change of state events
void TL2CAPSigState::Enter(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	}

void TL2CAPSigState::Exit(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	}	

// Signal command queue event
void TL2CAPSigState::PendingCommandsDrained(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}


// Timers
void TL2CAPSigState::ConfigurationTimerExpiry(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	PanicInState(EL2CAPUnexpectedSAPSignalHandlerEvent);
	}


// Helpers		
TBool TL2CAPSigState::IsChannelClosed() const
	{
	LOG_FUNC
	// Default is false.  Only true in closed state.
	return EFalse;
	}

TBool TL2CAPSigState::IsChannelOpen() const
	{
	// Only true in Open state.
	return EFalse;
	}

void TL2CAPSigState::HandleDisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const
	{
	LOG_FUNC
	aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitDisconnect));

	TInt err = aSignalHandler.ConstructDisconnectResponse(aId);
	if(err == KErrNone)
		{
		aSignalHandler.DrainPendingCommands();
		}
	else
		{
		// Failed to send the response.  Go directly into the closed state
		// and inform the SAP that disconnection is complete.
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.HandleLinkError();
		aSignalHandler.SAP()->ChannelClosed();
		}
	}

void TL2CAPSigState::HandleSAPDisconnect(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	TInt err = KErrDisconnected;
	
	if(aSignalHandler.RemotePort() != 0)
		{
		// We have a remote CID from a response with result pending, issue a disconnect request
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitDisconnect));
		err = aSignalHandler.ConstructDisconnectRequest();
		}	

	if(err != KErrNone)
		{
		// Failed to send the request or do not have a connection (without a valid CID). 
		// Go directly into the closed state and inform the SAP that disconnection is complete.
		aSignalHandler.HandleLinkError();
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.SAP()->ChannelClosed();
		}
	}

TInt TL2CAPSigState::SendConfigRequest(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	return aSignalHandler.ConstructConfigRequest();
	}

TInt TL2CAPSigState::SendConfigResponse(CL2CapSAPSignalHandler& aSignalHandler, TConfigResponseResult aResult) const
	{
	LOG_FUNC
	return aSignalHandler.ConstructConfigResponse(aSignalHandler.GetOutstandingRequestID(), 
	                                              HConfigureResponse::KNoConfigurationFlags,
	                                              aResult);
	}

void TL2CAPSigState::DisconnectWithError(CL2CapSAPSignalHandler& aSignalHandler, TInt aError) const
	{
	aSignalHandler.SetSignalHandlerErrorCode(aError);
	aSignalHandler.SetSignalHandlerErrorAction(MSocketNotify::EErrorAllOperations);
	aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitDisconnect));
	TInt err = aSignalHandler.ConstructDisconnectRequest();
	if(err != KErrNone)
		{
		// Failed to send the request. Call the Error method, this
		// will move the state to closed and forward the error to
		// the SAP.
		Error(aSignalHandler, aError, MSocketNotify::EErrorAllOperations);
		}
	}


/**
Calls the appropriate panic function to encode the panic
code with the current state identifier.
@param aPanic The panic code that the state is panicking with.
*/
void TL2CAPSigState::PanicInState(TL2CAPPanic aPanic) const
	{
	LOG_FUNC
	Panic(aPanic, iFactory.StateIndex(this));
	}
	
/*************************************************************************/
// Implementation of TL2CAPStateClosed
//
// Starting state and state re-entered when L2Cap logical link has been disconnected.
//
TL2CAPSigStateClosed::TL2CAPSigStateClosed(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigState(aFactory)
	{
	LOG_FUNC
	}

void TL2CAPSigStateClosed::OpenChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// The SAP has requested a channel be opened.
	// Override all LPMs to try and avoid signalling timeouts
	aSignalHandler.OverrideLPM();

	TInt err = aSignalHandler.ChannelConfig().UpdateLocalConfigWithEntityCapabilities();
	if (err == KErrNone)
		{
		// Get a free source CID for this channel to use.
		TL2CAPPort port;
		err = aSignalHandler.Mux().GetFreeCID(port);
		if(err == KErrNone)
			{
			// Update local Port.
			aSignalHandler.SetLocalPort(port);
	
			// Active connection.  Send a Connect Request command.
			err = aSignalHandler.ConstructConnectionRequest();
			if(err == KErrNone)
				{
				aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConnectRsp));
				}
			}
		}

	if (err != KErrNone)
		{
		// The L2CAP connection could not be initiated.  Error the SAP.
		Error(aSignalHandler, err, MSocketNotify::EErrorConnect);
		}
	}

void TL2CAPSigStateClosed::ConnectRequestReceived(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	TInt err = KErrNone;
	// An ACL has been established.
	// Get a free source CID for this channel to use.
	TL2CAPPort port;
	err = aSignalHandler.Mux().GetFreeCID(port);
	if(err == KErrNone)
		{
		// Update local Port.
		aSignalHandler.SetLocalPort(port);

		// Override all LPMs to try and avoid signalling timeouts
		aSignalHandler.OverrideLPM();

		// Send a response with a 'pending' result and start security procedure.
		err = aSignalHandler.ConstructConnectionResponse(aSignalHandler.GetOutstandingRequestID(),
	                                                     EConnectPending,
		                                                 EConnectPendAuthorization);
		}

	if(err == KErrNone)
		{
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConnect));
		}
	else
		{
		// The L2CAP connection could not be initiated.  Error the SAP.
		Error(aSignalHandler, err, MSocketNotify::EErrorConnect);
		}
	}


void TL2CAPSigStateClosed::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Detach from the Mux.  Inform the SAP that disconnection is complete.
	aSignalHandler.DetachFromMux();
	aSignalHandler.SAP()->ChannelClosed();
	}

void TL2CAPSigStateClosed::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
								  		 HConfigureRequest* aConfigRequest) const
	{
	LOG_FUNC
	aSignalHandler.SendInvalidCIDCommandReject(aConfigRequest->ID(),
											   0,
											   aConfigRequest->DestinationCID());
	}

TBool TL2CAPSigStateClosed::IsChannelClosed() const
	{
	LOG_FUNC
	return ETrue;
	}

void TL2CAPSigStateClosed::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Cancel the configuration timer.
	aSignalHandler.CancelTimer();

	aSignalHandler.iOpenChannelRequestAwaitingPeerEntityConfig = EFalse;

	// Depending where we came from we may have applied either a
	// park override or an all LPM override.  We remove both these
	// overrides because it's safe to remove an override that 
	// hasn't been applied.
	// Note that although park is one of the LPMs the different
	// levels of override are managed separately, so an
	// UndoOverrideLPM does not remove an explicit park only 
	// override.
	aSignalHandler.UndoOverrideParkMode();
	aSignalHandler.UndoOverrideLPM();

	// If the signal handler has left the closed state, it will have been added
	// to the SH list in the Muxer.  Now that the closed state is being
	// re-entered, remove the handler from the Muxer queue.
	// NB It is safe to call this method multiple times.
	aSignalHandler.DetachFromMux();
	}

	
/************************************************************/
// Implementation of void TL2CAPSigStateWaitConnectRsp
//
// This represents an Active SAP that is waiting for a connection response
TL2CAPSigStateWaitConnectRsp::TL2CAPSigStateWaitConnectRsp(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigState(aFactory)
	{
	LOG_FUNC
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitConnectRsp::ConnectResponse(CL2CapSAPSignalHandler& aSignalHandler,
								 				   HConnectionResponse* aConnectionResponse) const

	{
	LOG_FUNC	
	switch(aConnectionResponse->Result())
		{
		case EConnectSuccess:
			aSignalHandler.SetRemotePort(aConnectionResponse->DestinationCID());
			aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfig));
			aSignalHandler.SAP()->ChannelOpened();
			break;

		case EConnectPending:
			aSignalHandler.SetRemotePort(aConnectionResponse->DestinationCID());
			break;	// No other action here - the ERTx timer has been started.

		default:
			// One of the failed, non-pending results
			
			// The connect has failed
			// We're re-connectable, so go back to orig state
			// NB Error now clears muxer and sets state to closed
			Error(aSignalHandler, ((aConnectionResponse->Result() == EConnectSecurityBlock) ? KErrL2CAPAccessRequestDenied : KErrCouldNotConnect), MSocketNotify::EErrorConnect);
			break;
		};
	}
	
void TL2CAPSigStateWaitConnectRsp::ConfigRequest(CL2CapSAPSignalHandler& /*aSignalHandler*/,
						 						 HConfigureRequest* /*aConfigRequest*/) const
	{
	LOG_FUNC
	// Override the default panicing action for this state as we may validly be
	// here.  This could happen if the remote has sent a connect response, followed
	// by this config request but we have not successfully processed the connect 
	// response.  This can happen when the response arrives after the rtx has
	// expired but we haven't yet resent the connect request.
	}

// Events from the Mux
void TL2CAPSigStateWaitConnectRsp::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{	
	// Disconnect the channel.	
	HandleSAPDisconnect(aSignalHandler);
	}

void TL2CAPSigStateWaitConnectRsp::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	//set the remote CID to 0
	aSignalHandler.SetRemotePort(TL2CAPPort(0));
	aSignalHandler.iAwaitConfigureChannelRequest = ETrue;
	}

/****************************************************************************/
// 
// Waiting for the SAP to indicate that the connection can be established.
TL2CAPSigStateWaitConnect::TL2CAPSigStateWaitConnect(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigState(aFactory)
	{
	LOG_FUNC
	}
	
// Events from the SAP
void TL2CAPSigStateWaitConnect::OpenChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC

	TInt updateConfigErr = aSignalHandler.ChannelConfig().UpdateLocalConfigWithEntityCapabilities();
	TInt sendErr = KErrNone;
	if (updateConfigErr == KErrNone)
		{
		// The connection has been allowed.  Send the connection response.
		sendErr = aSignalHandler.ConstructConnectionResponse(aSignalHandler.GetOutstandingRequestID(),
															 EConnectSuccess,
															 EConnectPendNoFurtherInfo);
		if (sendErr == KErrNone)
			{
			// Enter the initial config state and initiate local configuration.
			aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfig));
			if(!aSignalHandler.DelayConfigRequest())
				// We should never get in here! The timer, which allows the delay
				// should have been started on entry to the state "WaitConfig". 
				{
				aSignalHandler.ConfigureChannel();
				}
			}
		} // Success Response
	else
		{
		// We don't have a generic 'Just no' response code, 'No resources' comes closest.
		sendErr = aSignalHandler.ConstructConnectionResponse(aSignalHandler.GetOutstandingRequestID(),
														     EConnectNoResources,
															 EConnectPendNoFurtherInfo);
		if (sendErr == KErrNone)
			{
			// When the response is sent, PendingCommandsDrained() will call Error().
			aSignalHandler.SetSignalHandlerErrorCode(updateConfigErr);
			aSignalHandler.SetSignalHandlerErrorAction(MSocketNotify::EErrorConnect);
			aSignalHandler.DrainPendingCommands();
			}
		} // Failure response

	if (sendErr != KErrNone)
		{
		// The L2CAP connection could not be initiated.  Error the SAP.
		Error(aSignalHandler, sendErr, MSocketNotify::EErrorConnect);
		}
	}

// Events from the Mux
void TL2CAPSigStateWaitConnect::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC

	// Tell the peer that there has been a security error
	TInt err = aSignalHandler.ConstructConnectionResponse(aSignalHandler.GetOutstandingRequestID(),
	                                                      				EConnectSecurityBlock,
	                                                      				EConnectPendNoFurtherInfo);

	if(err == KErrNone)
		{
		aSignalHandler.DrainPendingCommands();
		}
	else
		{
		// Failed to send the response.  Go directly into the closed state
		// and inform the SAP that disconnection is complete.

		// HandleLinkError flushes all the signal handler queues.  This should be done
		// before making the transition to the EClosed state because the signal handler
		// will be detached from the mux it is bound to, at which point we expect there
		// to be no commands queued (as we will no longer be able to deal with them).
		aSignalHandler.HandleLinkError();
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.SAP()->ChannelClosed();
		}
	}

void TL2CAPSigStateWaitConnect::PendingCommandsDrained(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	if(aSignalHandler.SignalHandlerErrorCode() != KErrNone)
		{
		Error(aSignalHandler, aSignalHandler.SignalHandlerErrorCode(), aSignalHandler.SignalHandlerErrorAction());
		}
	else
		{
		// This will delete the signal handler if the SAP has detached.
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.SignalHandlerDisconnectedCanClose();
		}
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitConnect::DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const
	{
	LOG_FUNC
	// Call a common helper function to handle the Disconnect Request.
	HandleDisconnectRequest(aSignalHandler, aId);
	}

void TL2CAPSigStateWaitConnect::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	aSignalHandler.iAwaitConfigureChannelRequest = EFalse;
	}


/**************************************************************************/
// Implementation of TL2CAPSigStateConfigBase
//
// This represents a SAP that is currently undergoing 
// configuration on the link whether it be initial or reconfiguration
// This is the base class and will never itself be instantiated.

TL2CAPSigStateConfigBase::TL2CAPSigStateConfigBase(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigState(aFactory)
	{
	LOG_FUNC
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateConfigBase::DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const
	{
	LOG_FUNC
	// FIRST - Flush all three signalling queues
	aSignalHandler.HandleLinkError();
	
	// Call a common helper function to handle the Disconnect Request.
	// NB This will add a new command to the pending command list.
	HandleDisconnectRequest(aSignalHandler, aId);
	}

void TL2CAPSigStateConfigBase::Error(CL2CapSAPSignalHandler& aSignalHandler,
           							 TInt aErrorCode, 
           							 MSocketNotify::TOperationBitmasks aErrorAction) const
	{
	LOG_FUNC
	// Cancel the configuration timer.
	aSignalHandler.CancelTimer();

	// Call the base class error method.
	TL2CAPSigState::Error(aSignalHandler, aErrorCode, aErrorAction);
	}      							 	

// Events from the SAP
void TL2CAPSigStateConfigBase::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Disconnect the channel.
	HandleSAPDisconnect(aSignalHandler);
	}

TInt TL2CAPSigStateConfigBase::UpdateChannelConfig(CL2CapSAPSignalHandler& /*aSignalHandler*/, const TL2CapConfig& /*aAPIConfig*/) const
	{
	LOG_FUNC
	// Configuration is in progress.
	return KErrL2CAPConfigAlreadyInProgress;
	}

// Timer Events
void TL2CAPSigStateConfigBase::ConfigurationTimerExpiry(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	CloseChannel(aSignalHandler);
	}
	
// Common handling of Config Request and Response commands.
void TL2CAPSigStateConfigBase::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
											 HConfigureRequest* aConfigRequest,
											 CL2CAPSignalStateFactory::TSigStates aConfigSuccessState) const
	{
	LOG_FUNC
	RMBufChain unknownOptions;
	// Following the state machine, we know that OUR config request
	// has not yet been sent (or even queued) if the next state
	// (on success) is EWaitSendConfig
	TBool configRequestSent = aConfigSuccessState != CL2CAPSignalStateFactory::EWaitSendConfig;

	TInt err = aSignalHandler.ChannelConfig().HandleConfigRequest(*aConfigRequest, unknownOptions);
	if(err == KErrNone)
		{
		// Check if the Config Request has been segmented using the continuation flag.
		if(!(aConfigRequest->Flags() & KConfigOptionContinuationMask))
			{
			TConfigResponseResult result;
			err = aSignalHandler.ChannelConfig().ConfigRequestComplete(configRequestSent, result);
			if (err == KErrConfigRejected)
				{
				// 'State 2' negotiation channel mode conflict - disconnect when a request
				// for anything other than our preferred channel mode is received.
				// Don't send a Config Response, send a Disconnect Request immediately.
				DisconnectWithError(aSignalHandler, err);
				}
			else
				{
				if (err == KErrNone)
					{
					aSignalHandler.SetOutstandingRequestID(aConfigRequest->ID());
					// Send the config response.
					err = SendConfigResponse(aSignalHandler, result);
					if(err == KErrNone)
						{
						// Check if the peer config was acceptable.
						switch(result)
							{
							case EConfigSuccess:
								// Move to next state.
								aSignalHandler.SetState(iFactory.GetState(aConfigSuccessState));

								if (!configRequestSent && !aSignalHandler.iAwaitConfigureChannelRequest)
									{
									// The delay timer is there so that we don't send a
									// Config Req before receiving one from the remote
									// (as a workaround for some broken carkits, see 
									// DelayConfigRequest()). Since we've just received a
									// ConfigReq from the remote, we can stop the delay
									// timer and send our request if this is passive open
                                    // or remotely-initiated reconfiguration.
									// If it's an active open scenario then we need to wait
									// for security access request to get completed.
									aSignalHandler.CancelTimer();
									aSignalHandler.StartConfigurationTimer();
									aSignalHandler.ConfigureChannel();
									}
								break;

							case EConfigUnacceptableParams:
							case EConfigRejected:
								// Stay in current state.
								break;

							case EConfigUnknownOption: // impossible on this path
							default:
								__ASSERT_DEBUG(EFalse, PanicInState(EL2CAPInvalidConfigResponseCodeGenerated));
								break;
							}
						} // err == KErrNone
					} // err == KErrNone
				if (err)
					{
					// The L2CAP connection could not be initiated.  Error the SAP.
					Error(aSignalHandler, err, MSocketNotify::EErrorAllOperations);
					}
				} // err != KErrConfigRejected
			} // continuation bit not set
		else
			{
			// The config request has been segmented.  Send an empty Config Response
			// message and wait for the final config request containing a C-Flag set to zero.
			err = aSignalHandler.ConstructConfigResponse(aConfigRequest->ID(),
			                                             KConfigOptionContinuationMask,
			                                             EConfigSuccess);
			if(err != KErrNone)
				{
				// The L2CAP connection could not be initiated.  Error the SAP.
				Error(aSignalHandler, err, MSocketNotify::EErrorAllOperations);
				}
			}
		} // no error when parsing request command
	else
		{
		TBool segmentedRequest = aConfigRequest->Flags() & KConfigOptionContinuationMask;
		TConfigFlags responseFlags = segmentedRequest ? KConfigOptionContinuationMask :
														HConfigureResponse::KNoConfigurationFlags;

		// Switch on the reason for the incoming request parsing failure.
		switch(err)
			{
			case KErrBadPacketReceived:
				// The options could not be decoded.  Send a response
				// indicating that the config has been rejected.  Wait for
				// the peer to take action.
				err = aSignalHandler.ConstructConfigResponse(aConfigRequest->ID(),
				                                             responseFlags,
				                                             EConfigRejected);

				if(err != KErrNone)
					{
					Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorConnect);
					}
				break;

			case KErrConfigUnknownOptions:
				err = aSignalHandler.ConstructConfigResponse(aConfigRequest->ID(),
				                                             responseFlags,
				                                             EConfigUnknownOption,
				                                             unknownOptions);
				if(err != KErrNone)
					{
					Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorConnect);
					}
				break;

			default:
				// The L2CAP connection could not be initiated.  Error the SAP.
			    Error(aSignalHandler, err, MSocketNotify::EErrorConnect);
				break;
			}
		} // parsing not successful
	// Free the unknown options buffer.
	unknownOptions.Free();
	}

void TL2CAPSigStateConfigBase::ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
											 HConfigureResponse* aConfigResponse,
											 CL2CAPSignalStateFactory::TSigStates aConfigSuccessState) const
	{
	LOG_FUNC
	RMBufChain unknownOptions;
	TInt err = aSignalHandler.ChannelConfig().HandleConfigResponse(*aConfigResponse, unknownOptions);

	if(err == KErrNone)
		{
		// Check if the Config Response has been segmented using the continuation flag.
		if(!(aConfigResponse->Flags() & KConfigOptionContinuationMask))
			{
			// Check if the peer has accepted our config.
			CL2CapChannelConfig::TChannelConfigStatus status = aSignalHandler.ChannelConfig().LocalConfigurationStatus();

			// Modify the status according to the result in the response received.
			if(aConfigResponse->Results() == EConfigUnacceptableParams)
				{
				// If the result was unacceptable parameters always reconfigure.
				if(status == CL2CapChannelConfig::EChannelConfigComplete)
					{
					status = CL2CapChannelConfig::EChannelConfigOutstanding;
					}
				}
			else
				{
				if(aConfigResponse->Results() != EConfigSuccess)
					{
					// Either the peer has rejected the last request or some of the
					// parameters contained in the last request are not recognised.
					// Ensure the channel is disconnected.
					status = CL2CapChannelConfig::EChannelConfigFailed;
					}
				}

			switch(status)
				{
				case CL2CapChannelConfig::EChannelConfigComplete:
					// Configuration complete.  Switch to the next state.
					aSignalHandler.SetState(iFactory.GetState(aConfigSuccessState));
					break;

				case CL2CapChannelConfig::EChannelConfigOutstanding:
					// Local config should be restarted.
					// Send another Config Request and stay in this state.
					err = SendConfigRequest(aSignalHandler);
					if(err != KErrNone)
						{
						Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorAllOperations);
						}
					break;

				case CL2CapChannelConfig::EChannelConfigFailed:
					// Configuration has failed.
					// Store the error code and disconnect the channel.
					DisconnectWithError(aSignalHandler, KErrConfigRejected);
					break;

				default:
					__ASSERT_DEBUG(EFalse, PanicInState(EL2CAPInvalidPeerConfigCodeGenerated));
					break;
				};
			}
		// else - the config response has been segmented - wait for the final config
		// response containing a C-Flag set to zero.
		}
	else
		{
		Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorAllOperations);
		}
	// Free the unknown options buffer.	
	unknownOptions.Free();
	}

/**************************************************************************/
// Implementation of TL2CAPSigStateWaitConfig
//
// No config requests/responses sent or received.
// Waiting to receive peer L2Cap supported features.
//
TL2CAPSigStateWaitConfig::TL2CAPSigStateWaitConfig(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigStateConfigBase(aFactory) 
	{
	LOG_FUNC
	}

// Events from the SAP
void TL2CAPSigStateWaitConfig::ConfigureChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Local config can now be initiated.  The local config requirements must
	// take into account the peer entity capabilities.
	__ASSERT_DEBUG(aSignalHandler.ChannelConfig().IsPeerInfoDefined(),
				   Panic(EL2CAPFecConfigAttemptWithoutPeerInfo));

	if(!aSignalHandler.DelayConfigRequest())
		{
		// Send our config request.
		TInt err = SendConfigRequest(aSignalHandler);
		if(err == KErrNone)
			{
			aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfigReqRsp));
			}
		else
			{
			Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorAllOperations);
			}
		}
	// Else wait until the delay timer expires.
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitConfig::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
								  			 HConfigureRequest* aConfigRequest) const
	{
	LOG_FUNC
	// Call the config base class helper method to process this command.
	TL2CAPSigStateConfigBase::ConfigRequest(aSignalHandler, aConfigRequest, CL2CAPSignalStateFactory::EWaitSendConfig);
	}

// Change of state events
void TL2CAPSigStateWaitConfig::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Cancel the Config timer if it's running
	aSignalHandler.CancelTimer();
	
	// First start the ConfigReq sending delay timer (see DelayConfigRequest() for why).
	// It's short lived, we'll start the proper L2CAP config timer when it expires.
	aSignalHandler.StartConfigRequestDelayTimer();
	}

/**************************************************************************/
// Implementation of EWaitSendConfig
//
// Remote's config request received and responded to with 'success'.
// Waiting to send outgoing config request...
// ...waiting to receive peer L2Cap supported features.
//
TL2CAPSigStateWaitSendConfig::TL2CAPSigStateWaitSendConfig(const CL2CAPSignalStateFactory& aFactory)
  : TL2CAPSigStateConfigBase(aFactory) 
	{
	LOG_FUNC
	}
	
// Events from the SAP
void TL2CAPSigStateWaitSendConfig::ConfigureChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Local config can now be initiated.  The local config requirements must
	// take into account the peer entity capabilities.
	__ASSERT_DEBUG(aSignalHandler.ChannelConfig().IsPeerInfoDefined(),
				   Panic(EL2CAPFecConfigAttemptWithoutPeerInfo));

	// Send our config request.
	TInt err = SendConfigRequest(aSignalHandler);
	if(err == KErrNone)
		{
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfigRsp));
		}						
	else
		{
		Error(aSignalHandler, KErrCouldNotConnect, MSocketNotify::EErrorAllOperations);
		}
	}

/**************************************************************************/
// Implementation of TL2CAPSigStateWaitConfigReqRsp
//
// Outgoing config request sent.
// Waiting for remote's config request, and remote's config response.
//
TL2CAPSigStateWaitConfigReqRsp::TL2CAPSigStateWaitConfigReqRsp(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigStateConfigBase(aFactory) 
	{
	LOG_FUNC
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitConfigReqRsp::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
								  	  			   HConfigureRequest* aConfigRequest) const
	{
	LOG_FUNC
	// Call the config base class helper method to process this command.
	TL2CAPSigStateConfigBase::ConfigRequest(aSignalHandler, aConfigRequest, CL2CAPSignalStateFactory::EWaitConfigRsp);
	}

void TL2CAPSigStateWaitConfigReqRsp::ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
					  		  	                    HConfigureResponse* aConfigResponse) const
	{
	LOG_FUNC
	// Call the config base class helper method to process this command.
	TL2CAPSigStateConfigBase::ConfigResponse(aSignalHandler, aConfigResponse, CL2CAPSignalStateFactory::EWaitConfigReq);
	}

/**************************************************************************/
// Implementation of EWaitConfigRsp
//
// Remote's config request received and responded to with 'success'.
// Outgoing config request sent.
// Waiting for remote's config response.
//
TL2CAPSigStateWaitConfigRsp::TL2CAPSigStateWaitConfigRsp(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigStateConfigBase(aFactory) 
	{
	LOG_FUNC
	}

void TL2CAPSigStateWaitConfigRsp::ConfigResponse(CL2CapSAPSignalHandler& aSignalHandler,
					  		  	                 HConfigureResponse* aConfigResponse) const
	{
	LOG_FUNC
	// Call the config base class helper method to process this command.
	TL2CAPSigStateConfigBase::ConfigResponse(aSignalHandler, aConfigResponse, CL2CAPSignalStateFactory::EOpen);
	}

/**************************************************************************/
// Implementation of EWaitConfigReq
//
// Outgoing config request sent, and responded to with 'success'.
// Waiting for remote's config request.
//
TL2CAPSigStateWaitConfigReq::TL2CAPSigStateWaitConfigReq(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigStateConfigBase(aFactory) 
	{
	LOG_FUNC
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitConfigReq::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
								  	  		    HConfigureRequest* aConfigRequest) const
	{
	LOG_FUNC
	// Call the config base class helper method to process this command.
	TL2CAPSigStateConfigBase::ConfigRequest(aSignalHandler, aConfigRequest, CL2CAPSignalStateFactory::EOpen);
	}

	

/**************************************************************************/
// Implementation of void TL2CAPSigStateOpen
//
// L2Cap logical link open. Data can be sent and received.
//
TL2CAPSigStateOpen::TL2CAPSigStateOpen(const CL2CAPSignalStateFactory& aFactory)
	: TL2CAPSigState(aFactory) 
	{
	LOG_FUNC
	}

void TL2CAPSigStateOpen::ConfigRequest(CL2CapSAPSignalHandler& aSignalHandler,
								  	   HConfigureRequest* aConfigRequest) const
	{
	LOG_FUNC
	// Starts of reconfiguration instigated by remote device
	aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfig));

	// Inform the data plane [via the SAP] that reconfig is in progress and data
	// should not be sent until it completes.
	aSignalHandler.ReconfiguringChannel();
	static_cast<void>(aSignalHandler.HandleConfigureRequest(aConfigRequest));
	}

TInt TL2CAPSigStateOpen::UpdateChannelConfig(CL2CapSAPSignalHandler& aSignalHandler, const TL2CapConfig& aAPIConfig) const
	{
	LOG_FUNC
	TBool reconfigRequired = EFalse;
	TInt rerr = aSignalHandler.ChannelConfig().UpdateConfigAPIChange(aAPIConfig, reconfigRequired);
	if (reconfigRequired && rerr == KErrNone)
		{
		aSignalHandler.ReconfiguringChannel();
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfig));
		aSignalHandler.ConfigureChannel();
		rerr = KErrL2CAPConfigPending;
		}
	return rerr;
	}


void TL2CAPSigStateOpen::CloseChannel(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Disconnect the channel.
	HandleSAPDisconnect(aSignalHandler);
	}
	
void TL2CAPSigStateOpen::DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const
	{
	LOG_FUNC
	// A Disconnect Request has been received and therefore no
	// more data can be sent.  Close the outgoing SDU queue.
	aSignalHandler.SAP()->CloseOutgoingSDUQueue();

	// Call a common helper function to handle the Disconnect Request.
	HandleDisconnectRequest(aSignalHandler, aId);
	}

// "Artificial" state events
void TL2CAPSigStateOpen::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC

	TBool downgrade = EFalse;
	TInt err = aSignalHandler.ChannelConfig().FecNegotiator().CheckNegotiatedChannelMode(downgrade);
	if (err != KErrNone)
		{
		DisconnectWithError(aSignalHandler, err);
		}
	else if (downgrade)
   		{
		LOG(_L("CL2CapSigStateOpen::Enter doing reconfig"));
		// now get reconfiguring
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EWaitConfig));
		aSignalHandler.ConfigureChannel();
		}
	else
		{
		// Depending where we came from we may have applied either a
		// park override or an all LPM override.  We remove both these
		// overrides because it's safe to remove an override that 
		// hasn't been applied.
		// Note that although park is one of the LPMs the different
		// levels of override are managed separately, so an
		// UndoOverrideLPM does not remove an explicit park only 
		// override.
		aSignalHandler.UndoOverrideParkMode();
		aSignalHandler.UndoOverrideLPM();

		// Cancel the configuration timer.
		aSignalHandler.CancelTimer();

		// Inform the SAP that the channel is now configured and ready to use.
		aSignalHandler.SAP()->ChannelConfigured(aSignalHandler.ChannelConfig(),
		                                        aSignalHandler.Mux(),
	    	                                    aSignalHandler.LocalPort(),
	        	                                aSignalHandler.RemotePort());
		}
	}

TBool TL2CAPSigStateOpen::IsChannelOpen() const
	{
	return ETrue;
	}

TInt TL2CAPSigStateOpen::GetNegotiatedChannelMode(const CL2CapSAPSignalHandler& aSignalHandler, TL2CapChannelMode& aMode) const
	{
	return aSignalHandler.ChannelConfig().GetChannelMode(aMode);
	}

/**************************************************************************/
// Implementation of void TL2CAPSigStateWaitDisconnect
//
// Logical link still exists but is waiting to be disconnected. 
// NB Signalling can still be received, and should be handled.
//
TL2CAPSigStateWaitDisconnect::TL2CAPSigStateWaitDisconnect(const CL2CAPSignalStateFactory& aFactory) 
	: TL2CAPSigState(aFactory) 
	{
	LOG_FUNC
	}

// L2CAP commands received from the peer.
void TL2CAPSigStateWaitDisconnect::DisconnectRequest(CL2CapSAPSignalHandler& aSignalHandler, TUint8 aId) const
	{
	LOG_FUNC
	// We may have a ConfigReq awaiting response. This DisconnectReq may be coming from
	// a State 2 peer that didn't like our ConfigReq and disconnected immediately instead
	// of sending a ConfigRsp. Which means we should flush the ConfigReq so that we don't
	// wait forever. We can delete all commands in WAIT_DISCONNECT anyway:
	// - the only response we really wait for in WAIT_DISCONNECT is the DisconnectRsp,
	//   but since peer has just sent us a DisconnectReq on its own, we know it's closing
	//   anyway and it doesn't matter if it's received our DisconnectReq;
	// - the rest of the commands - Connect, Configure - can be just flushed - they don't
	//   matter once we enter the disconnection path.
	aSignalHandler.FlushAllQueues();

	// Call a common helper function to handle the Disconnect Request.
	HandleDisconnectRequest(aSignalHandler, aId);
	}
	
void TL2CAPSigStateWaitDisconnect::DisconnectResponse(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// We've got a disconnect response, we're done.  Any remaing stuff on the queue
	// is redundant, so ditch it and close synchronously.
	aSignalHandler.FlushAllQueues();
	
	// If an error condition has been recorded forward the error 
	// to the SAP.
	if(aSignalHandler.SignalHandlerErrorCode() != KErrNone)
		{
		Error(aSignalHandler, aSignalHandler.SignalHandlerErrorCode(), aSignalHandler.SignalHandlerErrorAction());
		}
	else
		{
		// This will delete the signal handler if the SAP has detached.
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.SignalHandlerDisconnectedCanClose();
		}

	}

// "Artificial" state events
void TL2CAPSigStateWaitDisconnect::PendingCommandsDrained(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// If an error condition has been recorded forward the error 
	// to the SAP.
	if(aSignalHandler.SignalHandlerErrorCode() != KErrNone)
		{
		Error(aSignalHandler, aSignalHandler.SignalHandlerErrorCode(), aSignalHandler.SignalHandlerErrorAction());
		}
	else
		{
		// This will delete the signal handler if the SAP has detached.
		aSignalHandler.SetState(iFactory.GetState(CL2CAPSignalStateFactory::EClosed));
		aSignalHandler.SignalHandlerDisconnectedCanClose();
		}
	}
		
void TL2CAPSigStateWaitDisconnect::Enter(CL2CapSAPSignalHandler& aSignalHandler) const
	{
	LOG_FUNC
	// Only override park for disconnect.  We need to be able
	// to send the signalling, but we don't care if it takes
	// ages.  No point starting a sniff bun-fight.
	aSignalHandler.OverrideParkMode();

	// Cancel the configuration timer.
	aSignalHandler.CancelTimer();
	}

void TL2CAPSigStateWaitDisconnect::CloseChannel(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	// Already disconnecting.
	}

TInt TL2CAPSigStateWaitDisconnect::UpdateChannelConfig(CL2CapSAPSignalHandler& /*aSignalHandler*/, const TL2CapConfig& /*aAPIConfig*/) const
	{
	LOG_FUNC
	// Disconnecting - no point.
	return KErrDisconnected;
	}

void TL2CAPSigStateWaitDisconnect::ConnectResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/,
								 HConnectionResponse* /*aConnectionResponse*/) const
	{
	LOG_FUNC
	// Could be here if we disconnect between receiving a pending connect response and
	// a final one. 
	}

void TL2CAPSigStateWaitDisconnect::ConfigRequest(CL2CapSAPSignalHandler& /*aSignalHandler*/,
										  		 HConfigureRequest* /*aConfigRequest*/) const
	{
	LOG_FUNC
	// This often happens under the following scenario:
	// 1. Connection Req/Rsp are exchanged and the local entity sends a Config Req.
	// 2. Local entity sends Config Req first.
	// 3. Remote responds with Config Rsp Unacceptable Parameters which is not acceptable
	//    for us and hence we decide to disconnect, send a DisconnectReq and enter this
	//    state (WAIT_DISCONNECT).
	// 4. Remote legitimately sends ConfigReq before it gets the DisconnectReq and we end
	//    up here.
	//
	// The state tables in the spec say we should respond with a CmdRej Invalid CID but it is
	// unneeded and introduces a bit of unnecessary chaos. Let's ignore this ConfigReq and wait
	// until the remote acknowledges disconnection. If it retransmits the ConfigReq after that,
	// we'll then respond with a CmdRej Invalid CID which will be closer to the truth and won't
	// cause more unnecessary traffic.
	}

void TL2CAPSigStateWaitDisconnect::ConfigResponse(CL2CapSAPSignalHandler& /*aSignalHandler*/,
 	                					   		  HConfigureResponse* /*aConfigResponse*/) const
	{
	LOG_FUNC
	// Could well be in this state - just drop.
	}

void TL2CAPSigStateWaitDisconnect::ConfigureChannel(CL2CapSAPSignalHandler& /*aSignalHandler*/) const
	{
	LOG_FUNC
	// This may be called if an Information Request times out
	// waiting for an Information Response, and we have since
	// moved into the Wait Disconnect state.  Therefore don't
	// panic, however there is nothing else we can do here.
	}

	
/**************************************************************************/
// CL2CAPSignalStateFactory implementation
//
// That which creates the states, and manages state change
//
CL2CAPSignalStateFactory::CL2CAPSignalStateFactory()
	{
	LOG_FUNC
	}

CL2CAPSignalStateFactory::~CL2CAPSignalStateFactory()
	{
	LOG_FUNC
	// Destroy all the state objects
	iSigStates.DeleteAll();
	}
	
CL2CAPSignalStateFactory* CL2CAPSignalStateFactory::NewL()
	{
	LOG_STATIC_FUNC
	CL2CAPSignalStateFactory* factory= new (ELeave) CL2CAPSignalStateFactory();
	CleanupStack::PushL(factory);
	factory->ConstructL();
	CleanupStack::Pop();
	return factory;
	}
	
void CL2CAPSignalStateFactory::ConstructL()
	{
	LOG_FUNC
	// Create all the new states
	iSigStates[EClosed] 			= new (ELeave) TL2CAPSigStateClosed(*this);
	iSigStates[EWaitConnectRsp] 	= new (ELeave) TL2CAPSigStateWaitConnectRsp(*this);
	iSigStates[EWaitConnect] 		= new (ELeave) TL2CAPSigStateWaitConnect(*this);
	iSigStates[EWaitConfig] 		= new (ELeave) TL2CAPSigStateWaitConfig(*this);
	iSigStates[EWaitSendConfig] 	= new (ELeave) TL2CAPSigStateWaitSendConfig(*this);
	iSigStates[EWaitConfigReqRsp]	= new (ELeave) TL2CAPSigStateWaitConfigReqRsp(*this);
	iSigStates[EWaitConfigRsp] 		= new (ELeave) TL2CAPSigStateWaitConfigRsp(*this);
	iSigStates[EWaitConfigReq] 		= new (ELeave) TL2CAPSigStateWaitConfigReq(*this);
	iSigStates[EOpen] 				= new (ELeave) TL2CAPSigStateOpen(*this);
	iSigStates[EWaitDisconnect]		= new (ELeave) TL2CAPSigStateWaitDisconnect(*this);
	
	LOG1(_L("L2CAP SigState Closed = 0x%08x"), iSigStates[EClosed]);
	LOG1(_L("L2CAP SigState WaitConnectRsp = 0x%08x"), iSigStates[EWaitConnectRsp]);
	LOG1(_L("L2CAP SigState WaitConnect = 0x%08x"), iSigStates[EWaitConnect]);
	LOG1(_L("L2CAP SigState WaitConfig = 0x%08x"), iSigStates[EWaitConfig]);
	LOG1(_L("L2CAP SigState WaitSendConfig= 0x%08x"), iSigStates[EWaitSendConfig]);
	LOG1(_L("L2CAP SigState WaitConfigReqRsp = 0x%08x"), iSigStates[EWaitConfigReqRsp]);
	LOG1(_L("L2CAP SigState WaitConfigRsp = 0x%08x"), iSigStates[EWaitConfigRsp]);
	LOG1(_L("L2CAP SigState WaitConfigReq = 0x%08x"), iSigStates[EWaitConfigReq]);
	LOG1(_L("L2CAP SigState Open = 0x%08x"), iSigStates[EOpen]);
	LOG1(_L("L2CAP SigState WaitDisconnect = 0x%08x"), iSigStates[EWaitDisconnect]);
	}	

TL2CAPSigState& CL2CAPSignalStateFactory::GetState(const TSigStates aState) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EMaxState, Panic(EL2CAPMuxerStateOutOfBounds));
	return *iSigStates[aState];
	}
	
TInt CL2CAPSignalStateFactory::StateIndex(const TL2CAPSigState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < EMaxState; state++)
		{
		if (iSigStates[state] == aState)
			{
			return state;
			}
		}
	
	return KL2UnknownState;
	}

