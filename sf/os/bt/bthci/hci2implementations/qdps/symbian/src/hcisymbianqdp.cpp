// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "hcisymbianqdp.h"
#include "hcieventmodifiable.h"
#include <bluetooth/hcicommandqitem.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/command.h>
#include <bluetooth/hci/event.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/disconnectioncompleteevent.h>
#include <bluetooth/hci/readclockoffsetevent.h>
#include <bluetooth/hci/authenticationcompleteevent.h>
#include <bluetooth/hci/readlocalversioninfocompleteevent.h>
#include <bluetooth/hci/writelinkpolicysettingscommand.h>
#include <bluetooth/hci/readlinkpolicysettingscommand.h>
#include <bluetooth/hci/readlmphandlecommand.h>
#include <bluetooth/hci/rolediscoverycommand.h>
#include <bluetooth/hci/sniffsubratingcommand.h>
#include <bluetooth/hci/flushcommand.h>
#include <bluetooth/hci/readautomaticflushtimeoutcommand.h>
#include <bluetooth/hci/writeautomaticflushtimeoutcommand.h>
#include <bluetooth/hci/readtransmitpowerlevelcommand.h>
#include <bluetooth/hci/readlinksupervisiontimeoutcommand.h>
#include <bluetooth/hci/writelinksupervisiontimeoutcommand.h>
#include <bluetooth/hci/readfailedcontactcountercommand.h>
#include <bluetooth/hci/resetfailedcontactcountercommand.h>
#include <bluetooth/hci/readlinkqualitycommand.h>
#include <bluetooth/hci/readrssicommand.h>
#include <bluetooth/hci/readafhchannelmapcommand.h>
#include <bluetooth/hci/readclockcommand.h>

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_QDP_SYMBIAN);
#endif

#ifdef _DEBUG
PANICCATEGORY("qdpsymbia");
#endif // _DEBUG

void AppendConnectionHandle(TDes8& aDes, THCIConnectionHandle aConnectionHandle)
	{
	LOG_STATIC_FUNC
	THCIConnHandle connHandle = aConnectionHandle.ConnHandle();
	LOG1(_L8("Appending connection handle = 0x%04x"), connHandle);
	TUint8 val[2] = {connHandle & 0xff, connHandle >> 8};
	aDes.Append(val, 2);
	}

/*static*/ CHCISymbianQdp* CHCISymbianQdp::NewL()
	{
	LOG_STATIC_FUNC
	
	CHCISymbianQdp* self = new (ELeave) CHCISymbianQdp();
	return self;
	}

// Private constructor.
CHCISymbianQdp::CHCISymbianQdp()
	{
	LOG_FUNC
	}

TAny* CHCISymbianQdp::Interface(TUid aUid)
	{
	TAny* ret = NULL;
	
	switch(aUid.iUid)
		{
		case KHCICmdQueueDecisionInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCICmdQueueDecisionInterface*>(this));
			break;
		case KHCICmdQueueDecisionEventModifierInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCICmdQueueEventModifierInterface*>(this));
			break;
		case KHCICmdQueueUtilityUserUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCICmdQueueUtilityUser*>(this));
			break;
		default:
			break;
		};

	return ret;
	}

// MHCICmdQueueDecisionInterface
TBool CHCISymbianQdp::MhcqdiDoesCommandRequireWorkaround(const CHCICommandQItem& /* aParent */)
	{
	LOG_FUNC
	
	// No Workarounds required.
	return EFalse;
	}
	
CHCICommandQItem* CHCISymbianQdp::MhcqdiGetPreChildCommand(const CHCICommandQItem& /* aParent */, 
														   const CHCICommandQItem* /* aPreviousWorkaroundCmd */,
														   const THCIEventBase* /*aPreviousCmdResult*/)
	{
	LOG_FUNC
	
	// No Workarounds required (see MhcqdiDoesCommandRequireWorkaround), should never be called.
	return NULL;
	}

CHCICommandQItem* CHCISymbianQdp::MhcqdiGetPostChildCommand(const CHCICommandQItem& /* aParent */, 
															const CHCICommandQItem* /* aPreviousPostChild */, 
															const THCIEventBase* /*aPreviousCmdResult*/)
	{
	LOG_FUNC
	
	// No Workarounds required (see MhcqdiDoesCommandRequireWorkaround), should never be called.
	return NULL;
	}
	
THCIEventBase* CHCISymbianQdp::MhcqdiGetFakedUnsolicitedEvent(const CHCICommandQItem& /*aParent*/,
															  const THCIEventBase* /*aPreviousFakedEvent*/)
	{
	LOG_FUNC
	
	// No Workarounds required (see MhcqdiDoesCommandRequireWorkaround), should never be called.
	return NULL;
	}
	
void CHCISymbianQdp::MhcqdiCommandAboutToBeDeleted(const CHCICommandQItem& /*aDyingCmd*/)
	{
	LOG_FUNC
	
	// Notification function. No need to do anything.
	}
	
TInt CHCISymbianQdp::MhcqdiCanSend(CHCICommandQItem& /*aCommand*/, const TDblQue<const CHCICommandQItem>& aSentCommands)
	{
	LOG_FUNC

#ifdef SERIAL_LOW_POWER_MODE_REQUESTS
	if (!aSentCommands.IsEmpty())
		{
		THCIOpcode opcode = aSentCommands.Last()->Command().Opcode();
		// The following  commands are blocked to avoid ambiguity in matching mode change
		// events.  This ensures they are issued serially to the controller to prevent any confusion.
		// Note: This workaround currently resides in this Symbian QDP, but may require further
		// modification or placement depending on target hardware characteristics. This workaround
		// may not be required for all controllers.
		if(opcode == KHoldModeOpcode
		|| opcode == KSniffModeOpcode
		|| opcode == KExitSniffModeOpcode
		|| opcode == KSwitchRoleOpcode
		|| opcode == KParkModeOpcode
		|| opcode == KExitParkModeOpcode)
			{
			return EBlock;
			}
		}
#endif // SERIAL_LOW_POWER_MODE_REQUESTS
		
	// if no other issue then allow command queue to proceed   
	return EContinue;
	}
	
TUint CHCISymbianQdp::MhcqdiTimeoutRequired(const CHCICommandQItem& /* aCmdAboutToBeSent */)
	{
	LOG_FUNC
	
	// No timeout required.
	return MHCICmdQueueDecisionInterface::KNoTimeoutRequired;
	}
	
void CHCISymbianQdp::MhcqdiMatchedEventReceived(const THCIEventBase& aEvent, const CHCICommandQItem& /*aRelatedCommand*/)
	{
	LOG_FUNC
	
	// Cache the HCI version number of the controller. This allows
	// us to ignore errors from specific versions of controllers
	if(aEvent.EventCode() == ECommandCompleteEvent
	&& THCICommandCompleteEvent::Cast(aEvent).CommandOpcode() == KReadLocalVersionInfoOpcode)
		{
		const TReadLocalVersionInfoCompleteEvent& readLocalVersionCompleteEvent = TReadLocalVersionInfoCompleteEvent::Cast(aEvent);
		iHCIVersion = readLocalVersionCompleteEvent.Version();
		}
	}

void CHCISymbianQdp::MhcqemiMatchedEventReceived(THCIEventBase& aEvent, const CHCICommandQItem& aRelatedCommand)
	{
	LOG_FUNC
	
#ifdef IGNORE_INVALID_HCI_PARAMETER_ERROR_ON_SET_EVENT_MASK_ON_VERSION_1_1
	FixIgnoreInvalidHciParameterErrorOnSetEventMaskOnVersion1_1(aEvent);
#endif // IGNORE_INVALID_HCI_PARAMETER_ERROR_ON_SET_EVENT_MASK_ON_VERSION_1_1
	
#ifdef FAKE_COMPLETION_EVENTS_ON_DISCONNECTION
	FixFakeCompletionEventsOnDisconnection(aEvent);
#endif // FAKE_COMPLETION_EVENTS_ON_DISCONNECTION

#ifdef ADD_CONNECTION_HANDLE_FOR_TRUNCATED_INVALID_CONNECTION_HANDLE_ERROR_EVENTS
	FixAddConnectionHandleForTruncatedInvalidConnectionHandleErrorEvents(aEvent, &aRelatedCommand);
#endif // ADD_CONNECTION_HANDLE_FOR_TRUNCATED_INVALID_CONNECTION_HANDLE_ERROR_EVENTS

	// Don't forget to call the non-modifiable version of this function too
	MhcqdiMatchedEventReceived(aEvent, aRelatedCommand);
	}


MHCICmdQueueDecisionInterface::TCommandErroredAction CHCISymbianQdp::MhcqdiMatchedErrorEventReceived(const THCIEventBase& /*aErrorEvent*/, 
																									 const CHCICommandQItem& /*aRelatedCommand*/)
	{
	LOG_FUNC
	
	// Never resend.
	return MHCICmdQueueDecisionInterface::EContinueWithError;
	}
	
void CHCISymbianQdp::MhcqdiUnmatchedEventReceived(const THCIEventBase& /*aEvent*/)
	{
	LOG_FUNC
	
	// Notification function. No need to do anything.
	}

void CHCISymbianQdp::FixIgnoreInvalidHciParameterErrorOnSetEventMaskOnVersion1_1(THCIEventBase& aEvent)
	{
	LOG_FUNC
	// Some controllers supporting Bluetooth 1.1 return an EInvalidHCIParameter error
	// when SetEventMask is called. We still want to call SetEventMask and catch any actual
	// error in the stack (since it means that stack start up has failed).
	// In this case, stack start-up is fine, just ignore the returned EInvalidHCIParameter
	
	if(aEvent.ErrorCode() == EInvalidHCIParameter
	&& aEvent.EventCode() == ECommandCompleteEvent
	&& KSetEventMaskOpcode == THCICommandCompleteEvent::Cast(aEvent).CommandOpcode()
	&& iHCIVersion == EHWHCIv1_1)
		{
		LOG(_L8("Ignoring Invalid HCI Parameter error for Set Event Mask"));
		THCIEventModifiable& event = static_cast<THCIEventModifiable&>(aEvent);
		event.SetErrorCode(EOK);
		}
	}

void CHCISymbianQdp::FixFakeCompletionEventsOnDisconnection(THCIEventBase& aEvent)
	{
	LOG_FUNC
	// Some controllers fail to follow the HCI specification w.r.t. events on disconnection.
	// The specification indicates outstanding data on a connection handle to be considered
	// removed when a disconnection occurs.  Events however are not guarded by such text
	// and should always be returned (with an appropriate error code).
	// The command queue expects these events (as it is programmed to the spec) so if a
	// controller fails to perform the task, we have to make up the shortfall.  Currently
	// the following events are the ones that fail to be generated in the controllers. 
	// This issue has been observed with:
	//
	// * Authentication_Complete
	// * Read_Clock_Offset

	if(aEvent.EventCode() == EDisconnectionCompleteEvent)
		{
		const TDisconnectionCompleteEvent& disconnEvent = TDisconnectionCompleteEvent::Cast(aEvent);
		THCIConnectionHandle handle = disconnEvent.ConnectionHandle();
		THCIErrorCode reason = static_cast<THCIErrorCode>(disconnEvent.Reason());
		
		if(iProvider->FindOutstandingCommand(KAuthenticationRequestedOpcode))
			{
			LOG(_L8("Injecting Authentication Complete Event"));
			TAuthenticationCompleteEvent authenticationCompleteEvent(reason, handle, iEventModBuffer);
			iProvider->InjectEvent(authenticationCompleteEvent);
			}
		
		if(iProvider->FindOutstandingCommand(KReadClockOffsetOpcode))
			{
			LOG(_L8("Injecting Read Clock Offset Complete Event"));
			THCIClockOffset clockOffset = 0;
			TReadClockOffsetEvent readClockOffsetEvent(reason, handle, clockOffset, iEventModBuffer);
			iProvider->InjectEvent(readClockOffsetEvent);
			}
		}
	}

/**
Utility template class for FixAddConnectionHandleForTruncatedInvalidConnectionHandleErrorEvents function.
*/
template<class XCommandCompleteCommandClass>
struct AttemptToFixCommandCompleteEvent
	{
	static void CheckAndFix(THCICommandCompleteEvent& aEvent, const CHCICommandQItem* aRelatedCommand, TDes8& aNewBuffer, TInt aCorrectSize, TInt aExpectedSizeMissing)
		{
		LOG_STATIC_FUNC
		THCIEventModifiable& event = static_cast<THCIEventModifiable&>(static_cast<THCIEventBase&>(aEvent));
		// Check to see if the data is truncated - only apply the fix if it is
		if(event.EventData().Length() == (aCorrectSize-aExpectedSizeMissing))
			{
			// This is actually a best effort guess that the connection handle is missing.  Fixing this isn't simple because
			// we need a bigger buffer than we may have - so we have to create our own backing buffer for the event.
			LOG1(_L8("Modifying Command Complete event (opcode = 0x%04x) to add Connection Handle"), aEvent.CommandOpcode());
			aNewBuffer.FillZ(aCorrectSize); // ensure buffer is clean
			aNewBuffer.Copy(event.EventData());
			if(aRelatedCommand)
				{
				const XCommandCompleteCommandClass& cmd = static_cast<const XCommandCompleteCommandClass&>(aRelatedCommand->Command());
				AppendConnectionHandle(aNewBuffer, cmd.ConnectionHandle());
				}
			else
				{
				// we have no connection handle to insert in, so pick one that can
				// never be valid.
				AppendConnectionHandle(aNewBuffer, KInvalidConnectionHandle);
				}
			aNewBuffer.SetLength(aCorrectSize);
			event.EventData().Set(aNewBuffer); // update event to point to new buffer
			}
		// Ensure that now the event is correct.
		ASSERT_DEBUG(event.EventData().Length() == aCorrectSize);
		}
	};
	
void CHCISymbianQdp::FixAddConnectionHandleForTruncatedInvalidConnectionHandleErrorEvents(THCIEventBase& aEvent, const CHCICommandQItem* aRelatedCommand)
	{
	LOG_FUNC
	// Some controllers do not follow the HCI specification in that they do not always return
	// an event of the correct size - in this case controllers omit a connection handle field when
	// erroring with "invalid connection handle" error code.  One can argue about this but the
	// command queue is designed with the spec in mind so just adjust the events as appropriate.
	//
	// Notably command complete events are the events with issues; if a command status is used
	// then the error code is communicated without any other parameters, and the resulting events will 
	// not generally be generated (see FixFakeCompletionEventsOnDisconnection).
	// Current events that have observed (^) this issue (or could potentially have this issue (*)) are:
	//
	// ^ Command_Complete (Write_Link_Policy_Settings)
	// ^ Command_Complete (Read_Link_Policy_Settings)
	// * Command_Complete (Read_LMP_Handle)
	// ^ Command_Complete (Role_Discovery)
	// ^ Command_Complete (Sniff_Subrating)
	// * Command_Complete (Flush)
	// * Command_Complete (Read_Auto_Flush_Timeout)
	// * Command_Complete (Write_Auto_Flush_Timeout)
	// * Command_Complete (Read_Transmit_Power_Level)
	// * Command_Complete (Read_Link_Supervision_Timeout)
	// * Command_Complete (Write_Link_Supervision_Timeout)
	// * Command_Complete (Read_Failed_Contact_Counter)
	// * Command_Complete (Reset_Failed_Contact_Counter)
	// * Command_Complete (Read_Link_Quality)
	// * Command_Complete (Read_RSSI)
	// * Command_Complete (Read_AFH_Channel_Map)
	// * Command_Complete (Read_Clock)
	//
	// Only those actually observed as issues are included - the others are commented out for reference
	
	if(aEvent.ErrorCode() == ENoConnection)
		{
		if(aEvent.EventCode() == ECommandCompleteEvent)
			{
			THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
			// Macro to make the code more concise
			#define _CHECK_AND_FIX(_COMMAND_NAME, _CORRECT_SIZE, _MISSING_SIZE) \
				case K##_COMMAND_NAME##Opcode:\
					AttemptToFixCommandCompleteEvent<C##_COMMAND_NAME##Command>::CheckAndFix(completeEvent, aRelatedCommand, iEventModBuffer, (_CORRECT_SIZE), (_MISSING_SIZE));\
					break
			switch(completeEvent.CommandOpcode())
				{
			_CHECK_AND_FIX(WriteLinkPolicySettings,		 8, sizeof(THCIConnHandle));
			_CHECK_AND_FIX(ReadLinkPolicySettings,		10, sizeof(THCIConnHandle) + sizeof(TUint16));
			_CHECK_AND_FIX(RoleDiscovery,				 9, sizeof(THCIConnHandle) + sizeof(TUint8));
			_CHECK_AND_FIX(SniffSubrating,				 8, sizeof(THCIConnHandle));
			
			// These below are included as they potentially could have the same problem,
			// however in practice this issue has not been observed.
		//	_CHECK_AND_FIX(ReadLMPHandle,				13, sizeof(THCIConnHandle) + sizeof(TUint8) + sizeof(TUint32));
		//	_CHECK_AND_FIX(Flush,						 8, sizeof(THCIConnHandle) + sizeof(TUint8) + sizeof(TUint32));
		//	_CHECK_AND_FIX(ReadAutomaticFlushTimeout,	10, sizeof(THCIConnHandle) + sizeof(TUint16));
		//	_CHECK_AND_FIX(WriteAutomaticFlushTimeout,	 8, sizeof(THCIConnHandle));
		//	_CHECK_AND_FIX(ReadTransmitPowerLevel,		 9, sizeof(THCIConnHandle) + sizeof(TUint8));
		//	_CHECK_AND_FIX(ReadLinkSupervisionTimeout,	10, sizeof(THCIConnHandle) + sizeof(TUint16));
		//	_CHECK_AND_FIX(WriteLinkSupervisionTimeout,	 8, sizeof(THCIConnHandle));
		//	_CHECK_AND_FIX(ReadFailedContactCounter,	10, sizeof(THCIConnHandle) + sizeof(TUint16));
		//	_CHECK_AND_FIX(ResetFailedContactCounter,	 8, sizeof(THCIConnHandle));
		//	_CHECK_AND_FIX(ReadLinkQuality,				 9, sizeof(THCIConnHandle));
		//	_CHECK_AND_FIX(ReadRSSI,					 9, sizeof(THCIConnHandle) + sizeof(TUint8));
		//	_CHECK_AND_FIX(ReadAFHChannelMap,			19, sizeof(THCIConnHandle) + sizeof(TUint8) + 10);
		//	_CHECK_AND_FIX(ReadClock,					14, sizeof(THCIConnHandle) + sizeof(TUint32) + sizeof(TUint16));
			
			default:
				// just ignore
				break;
				}
			#undef _CHECK_AND_FIX
			}
		}
	}

void CHCISymbianQdp::MhcqemiUnmatchedEventReceived(THCIEventBase& aEvent)
	{
	LOG_FUNC
	
#ifdef FAKE_COMPLETION_EVENTS_ON_DISCONNECTION
	FixFakeCompletionEventsOnDisconnection(aEvent);
#endif // FAKE_COMPLETION_EVENTS_ON_DISCONNECTION
	
	MhcqdiUnmatchedEventReceived(aEvent);
	}
	
MHCICmdQueueDecisionInterface::TCommandTimedOutAction CHCISymbianQdp::MhcqdiCommandTimedOut(const CHCICommandQItem& /*aCommand*/,
																							const TDblQue<const CHCICommandQItem>& /*aSentCommands*/,
																							TUint /*aCurrentCommandCredits*/,
																							TUint& aCreditsToBeRefunded)
	{
	LOG_FUNC
	
	// No Timeout ever set, should never be called.
	aCreditsToBeRefunded = KHCIDefaultCmdCredits;
	return EContinueWithTimeoutEvent;
	}
	
void CHCISymbianQdp::MhcqdiSetPhysicalLinksState(const MPhysicalLinksState& /*aPhysicalLinkState*/)
	{
	LOG_FUNC
	}
	
void CHCISymbianQdp::MhcqdiSetHardResetInitiator(const MHardResetInitiator& /*aHardResetInitiator*/)
	{
	LOG_FUNC
	}
	
void CHCISymbianQdp::MhcqdiSetHCICommandQueue(MHCICommandQueue& /*aHCICommandQueue*/)
	{
	LOG_FUNC
	}

void CHCISymbianQdp::MhcqdiSetTimeouts(TUint /*aQueueStarvationTimeout*/,
									   TUint /*aMaxHciCommandTimeout*/)
	{
	LOG_FUNC
	}
	
TUint CHCISymbianQdp::MhcqdiReset()
	{
	LOG_FUNC
	
	// Return the initial number of command credits for the queue.
	return KHCIDefaultCmdCredits;
	}

void CHCISymbianQdp::MhcquuSetUtilitiesProvider(MHCICmdQueueUtilities& aProvider)
	{
	LOG_FUNC
	
	iProvider = &aProvider;
	}
