// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "secman.h"
#include "linkutil.h"

#include <bluetooth/hcicommandqueue.h>

#include <bluetooth/hci/event.h>
#include <bluetooth/hci/writesimplepairingmodecommand.h>
#include <bluetooth/hci/writesimplepairingdebugmodecommand.h>
#include <bluetooth/hci/iocapabilityrequestreplycommand.h>
#include <bluetooth/hci/iocapabilityrequestnegativereplycommand.h>
#include <bluetooth/hci/iocapabilityrequestnegativereplycommand.h>
#include <bluetooth/hci/userconfirmationrequestreplycommand.h>
#include <bluetooth/hci/userconfirmationrequestnegativereplycommand.h>
#include <bluetooth/hci/remoteoobdatarequestreplycommand.h>
#include <bluetooth/hci/remoteoobdatarequestnegativereplycommand.h>
#include <bluetooth/hci/readlocaloobdatacommand.h>

#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/commandstatusevent.h>

#include <bluetooth/hci/iocapabilityrequestevent.h>
#include <bluetooth/hci/iocapabilityresponseevent.h>
#include <bluetooth/hci/userconfirmationrequestevent.h>
#include <bluetooth/hci/userpasskeynotificationevent.h>
#include <bluetooth/hci/remoteoobdatarequestevent.h>
#include <bluetooth/hci/simplepairingcompleteevent.h>
#include <bluetooth/hci/keypressnotificationevent.h>
#include <bluetooth/hci/remotehostsupportedfeaturesnotificationevent.h>

#include <bluetooth/hci/writesimplepairingdebugmodecompleteevent.h>

#include <bluetooth/hci/readlocaloobdatacompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestreplycompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestnegativereplycompleteevent.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SECMAN);
#endif


// ------------------------------------------------------------------------
// class CSecManCommandController
// ------------------------------------------------------------------------

CSecManCommandController::CSecManCommandController(CBTSecMan& aSecMan)
	: iSecMan(aSecMan)
	{
	LOG_FUNC
	}

CSecManCommandController* CSecManCommandController::NewL(CBTSecMan& aSecMan)
	{
	LOG_STATIC_FUNC
	CSecManCommandController* self = CSecManCommandController::NewLC(aSecMan);
	CleanupStack::Pop(self);
	return self;
	}

CSecManCommandController* CSecManCommandController::NewLC(CBTSecMan& aSecMan)
	{
	LOG_STATIC_FUNC
	CSecManCommandController* self = new(ELeave) CSecManCommandController(aSecMan);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSecManCommandController::ConstructL()
	{
	LOG_FUNC
	}

CSecManCommandController::~CSecManCommandController()
	{
	LOG_FUNC
	ClearHCICommandQueue();
	}

void CSecManCommandController::SetHCICommandQueue(MHCICommandQueue& aCommandQueue)
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iCommandQueue, PANIC(KBTSecPanic, EBTSecCommandQueueAlreadyProvided));
	iCommandQueue = &aCommandQueue;
	}

void CSecManCommandController::ClearHCICommandQueue()
	{
	LOG_FUNC
	if(iCommandQueue)
		{
		iCommandQueue->MhcqRemoveAllCommands(*this);
		}
	iCommandQueue = NULL;
	}

MHCICommandQueue& CSecManCommandController::CommandQueue() const
	{
	LOG_FUNC
	__ASSERT_DEBUG(iCommandQueue, PANIC(KBTSecPanic, EBTSecCommandQueueNotAvailable));
	return *iCommandQueue;
	}

void CSecManCommandController::WriteSimplePairingDebugModeL(TUint8 aSimplePairingDebugMode)
	{
	LOG_FUNC
	// Ownership of cmd transfered
	CWriteSimplePairingDebugModeCommand* cmd = CWriteSimplePairingDebugModeCommand::NewL(aSimplePairingDebugMode);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::IOCapabilityRequestReplyL(const TBTDevAddr& aBDADDR,
											THCIIoCapability aIOCapability,
											THCIOobDataPresence aOOBDataPresent,
											THCIAuthenticationRequirement aAuthenticationRequirements)
	{
	LOG_FUNC
	// Ownership of cmd transfered
	CIOCapabilityRequestReplyCommand* cmd = CIOCapabilityRequestReplyCommand::NewL(aBDADDR, aIOCapability, aOOBDataPresent, aAuthenticationRequirements);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::IOCapabilityRequestNegativeReplyL(const TBTDevAddr& aBDADDR, TUint8 aReason)
	{
	// Ownership of cmd transfered
	CIOCapabilityRequestNegativeReplyCommand* cmd = CIOCapabilityRequestNegativeReplyCommand::NewL(aBDADDR, aReason);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::UserConfirmationRequestReplyL(const TBTDevAddr& aBDADDR)
	{
	LOG_FUNC
	// Ownership of cmd transfered
	CUserConfirmationRequestReplyCommand* cmd = CUserConfirmationRequestReplyCommand::NewL(aBDADDR);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::UserConfirmationRequestNegativeReplyL(const TBTDevAddr& aBDADDR)
	{
	LOG_FUNC
	// Ownership of cmd transfered
	CUserConfirmationRequestNegativeReplyCommand* cmd = CUserConfirmationRequestNegativeReplyCommand::NewL(aBDADDR);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::RemoteOOBDataRequestReplyL(const TBTDevAddr& aBDADDR,
											const TBluetoothSimplePairingHash& aC,
											const TBluetoothSimplePairingRandomizer& aR)
	{
	LOG_FUNC
	// Ownership of cmd transfered
	CRemoteOOBDataRequestReplyCommand* cmd = CRemoteOOBDataRequestReplyCommand::NewL(aBDADDR, aC, aR);
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::RemoteOOBDataRequestNegativeReplyL(const TBTDevAddr& aBDADDR)
	{
	LOG_FUNC
	CRemoteOOBDataRequestNegativeReplyCommand* cmd = CRemoteOOBDataRequestNegativeReplyCommand::NewL(aBDADDR);
	// Ownership of cmd transfered
	CommandQueue().MhcqAddCommandL(cmd, *this);
	}

void CSecManCommandController::MhcqcCommandErrored(TInt IF_FLOGGING(aErrorCode), const CHCICommandBase* __DEBUG_ONLY(aCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, PANIC(KBTSecPanic, EBTSecNoCommandAssociatedWithErrorEvent)); // this should never happen
	
	#ifdef _DEBUG
	LOG2(_L("error code:%d opcode:0x%04x"), aErrorCode, aCommand->Opcode());
	#else
	LOG1(_L("error code:%d"), aErrorCode);
	#endif
	}

// -----------------------------------------------------------------------------------------

// From MHCICommandQueueClient
void CSecManCommandController::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	switch(aEvent.EventCode())
		{
	case ECommandCompleteEvent:
		CommandCompleteEvent(aEvent);
		break;
		
	case ECommandStatusEvent:
		CommandStatusEvent(aEvent);
		break;
		
	case EIOCapabilityRequestEvent:
		IOCapabilityRequestEvent(aEvent);
		break;
		
	case EIOCapabilityResponseEvent:
		IOCapabilityResponseEvent(aEvent);
		break;
		
	case EUserConfirmationRequestEvent:
		UserConfirmationRequestEvent(aEvent);
		break;
		
	case EUserPasskeyRequestEvent:
		// Symbian devices are assumed to be DisplayYesNo statically.
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecBadUserPasskeyRequest));
		break;
		
	case EUserPasskeyNotificationEvent:
		UserPasskeyNotificationEvent(aEvent);
		break;
		
	case ERemoteOOBDataRequestEvent:
		RemoteOOBDataRequestEvent(aEvent);
		break;
		
	case EKeypressNotificationEvent:
		KeypressNotificationEvent(aEvent);
		break;
		
	case ESimplePairingCompleteEvent:
		SimplePairingCompleteEvent(aEvent);
		break;

	default:
		LOG1(_L("Warning!! Unknown Command Event Received (event code: %d)"), aEvent.EventCode());
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecUnknownHCIEvent));
		break;
		}
	}

void CSecManCommandController::IOCapabilityRequestEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TIOCapabilityRequestEvent& event = TIOCapabilityRequestEvent::Cast(aEvent);
	iSecMan.IOCapabilityRequestFromRemote(event.BDADDR());
	}

void CSecManCommandController::IOCapabilityResponseEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TIOCapabilityResponseEvent& event = TIOCapabilityResponseEvent::Cast(aEvent);
	iSecMan.IOCapabilityAskForResponse(event.BDADDR(), CastToIoCapability(event.IOCapability()), CastToOobDataPresence(event.OOBDataPresent()), CastToAuthenticationRequirements(event.AuthenticationRequirements()));
	}

void CSecManCommandController::UserConfirmationRequestEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TUserConfirmationRequestEvent& event = TUserConfirmationRequestEvent::Cast(aEvent);
	iSecMan.UserConfirmationRequest(event.BDADDR(), event.NumericValue());
	}

void CSecManCommandController::UserPasskeyNotificationEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TUserPasskeyNotificationEvent& event = TUserPasskeyNotificationEvent::Cast(aEvent);
	iSecMan.PasskeyNotification(event.BDADDR(), event.Passkey());
	}

void CSecManCommandController::RemoteOOBDataRequestEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TRemoteOOBDataRequestEvent& event = TRemoteOOBDataRequestEvent::Cast(aEvent);
	iSecMan.RemoteOOBDataRequest(event.BDADDR());
	}

void CSecManCommandController::SimplePairingCompleteEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TSimplePairingCompleteEvent& spevent = TSimplePairingCompleteEvent::Cast(aEvent);
	iSecMan.SimplePairingComplete(spevent.BDADDR(), aEvent.ErrorCode());
	}

void CSecManCommandController::KeypressNotificationEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TKeypressNotificationEvent& event = TKeypressNotificationEvent::Cast(aEvent);
	iSecMan.KeypressNotification(event.BDADDR(), event.NotificationType());
	}


void CSecManCommandController::CommandCompleteEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
	THCIOpcode opcode = completeEvent.CommandOpcode();
	THCIErrorCode hciErr = aEvent.ErrorCode();
	
	switch (opcode)
		{	
	case KRemoteOOBDataRequestReplyOpcode:
		RemoteOOBDataRequestReplyOpcode(completeEvent);
		break;
		
	case KRemoteOOBDataRequestNegativeReplyOpcode:
		RemoteOOBDataRequestNegativeReplyOpcode(completeEvent);
		break;
		
	case KIOCapabilityRequestReplyOpcode:
		// Check bdaddr
		break;
		
	case KWriteSimplePairingDebugModeOpcode:
		WriteSimplePairingDebugModeOpcode(completeEvent);
		break;
	
	// Need to handle link key request reply...
	// Need to complete link key request negative reply
	// Pin code request reply
	case KIOCapabilityRequestNegativeReplyOpcode:
	case KUserConfirmationRequestReplyOpcode:
		// check bdaddr.
	case KUserConfirmationRequestNegativeReplyOpcode:
		// check bdadrr
	case KUserPasskeyRequestReplyOpcode:
	case KUserPasskeyRequestNegativeReplyOpcode:
		// Catch all the events we do not handle
		LOG1(_L("Warning!! Unhandled Command Complete Event (opcode: %d)"), opcode);
		break;
	
	default:
		LOG2(_L("Link [HCIFacade_Events.cpp]: Warning: Unknown Command complete event! Opcode %d error code %d"), opcode, hciErr);
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecUnknownHCICommandCompleteOpcode));
		break;
		}
	}

void CSecManCommandController::CommandStatusEvent(const THCIEventBase& aEvent)
	{
	LOG_FUNC
	const TCommandStatusEvent& commandStatusEvent = TCommandStatusEvent::Cast(aEvent);
	THCIOpcode opcode = commandStatusEvent.CommandOpcode();
	THCIErrorCode hciErr = commandStatusEvent.ErrorCode();
	}

void CSecManCommandController::RemoteOOBDataRequestReplyOpcode(const THCICommandCompleteEvent& aCompleteEvent)
	{
	LOG_FUNC
	const TRemoteOOBDataRequestReplyCompleteEvent& event = TRemoteOOBDataRequestReplyCompleteEvent::Cast(aCompleteEvent);
	iSecMan.RemoteOOBDataRequestComplete(event.BDADDR());
	}

void CSecManCommandController::RemoteOOBDataRequestNegativeReplyOpcode(const THCICommandCompleteEvent& aCompleteEvent)
	{
	LOG_FUNC
	const TRemoteOOBDataRequestNegativeReplyCompleteEvent& event = TRemoteOOBDataRequestNegativeReplyCompleteEvent::Cast(aCompleteEvent);
	iSecMan.RemoteOOBDataRequestComplete(event.BDADDR());
	}

void CSecManCommandController::WriteSimplePairingDebugModeOpcode(const THCICommandCompleteEvent& aCompleteEvent)
	{
	LOG_FUNC
	const TWriteSimplePairingDebugModeCompleteEvent& event = TWriteSimplePairingDebugModeCompleteEvent::Cast(aCompleteEvent);
	if(event.ErrorCode() == EOK)
		{
		iSecMan.DebugModeChanged(ETrue);
		}
	// ignore errors - debug mode remains the same
	}
	
THCIIoCapability CSecManCommandController::CastToIoCapability(TUint8 aIOCapability)
	{
	LOG_STATIC_FUNC
	switch(aIOCapability)
		{
	case EIOCapsDisplayOnly:
	case EIOCapsDisplayYesNo:
	case EIOCapsKeyboardOnly:
	case EIOCapsNoInputNoOutput:
		// All valid understood values.
		break;
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecUnexpectedIoCapability));
		break;
		}
	return static_cast<THCIIoCapability>(aIOCapability);
	}

THCIOobDataPresence CSecManCommandController::CastToOobDataPresence(TUint8 aOOBDataPresent)
	{
	LOG_STATIC_FUNC
	switch(aOOBDataPresent)
		{
	case EOOBDataNotPresent:
	case EOOBDataPresent:
		// All valid understood values.
		break;
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecUnexpectedOobDataPresence));
		break;
		}
	return static_cast<THCIOobDataPresence>(aOOBDataPresent);
	}
	
THCIAuthenticationRequirement CSecManCommandController::CastToAuthenticationRequirements(TUint8 aAuthenticationRequirements)
	{
	LOG_STATIC_FUNC
	switch(aAuthenticationRequirements)
		{
	case EMitmNotReqNoBonding:
	case EMitmReqNoBonding:
	case EMitmNotReqDedicatedBonding:
	case EMitmReqDedicatedBonding:
	case EMitmNotReqGeneralBonding:
	case EMitmReqGeneralBonding:
		// All valid understood values.
		break;
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecUnexpectedAuthenticationRequirements));
		break;
		}
	return static_cast<THCIAuthenticationRequirement>(aAuthenticationRequirements);
	}

