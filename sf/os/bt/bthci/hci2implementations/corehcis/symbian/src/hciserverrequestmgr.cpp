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

#include "hciserverrequestmgr.h"
#include "HciCorePluginImpl.h"
#include "corehciutil.h"

#include <bluetooth/hci/hciipc.h>
#include <bluetooth/hci/vendordebugcommand.h>
#include <bluetooth/hci/vendordebugcompleteevent.h>
#include <bluetooth/hci/vendordebugevent.h>

#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/btpowercontrol.h>
#include <bluetooth/dutmode.h>
#include <bluetooth/a2dpoptimisation.h>
#include <bluetooth/a2dpoptimisationparams.h>

#include <bluetooth/logger.h>
#include <bluetooth/btpowercontrolmessages.h>

#include <es_sock.h>
#include <bt_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCISERVER);
#endif

const TInt KHCIServerIpcDesOutParam = 2;

void CHCIServerRequestManager::BlockAsync(CHCISession& aSession, const RMessage2& aMessage)
	{
	LOG_FUNC

	iAsyncTaskMessage = aMessage;
	iAsyncTaskSession = &aSession;
	iBlockMask |= MaskBit();
	}
	
void CHCIServerRequestManager::CompleteAsyncTask(TInt aError)
	{
	LOG_FUNC
	
	//check message is not already complete
	if(iAsyncTaskMessage.Handle())
		{
		iAsyncTaskMessage.Complete(aError);
		}
	else
		{
		__ASSERT_DEBUG(EFalse, PANIC(KHciServerPanicCat, EMessageAlreadyCompleted));
		}

	// Remove Async block		
	iAsyncTaskSession = 0;
	iBlockMask &= ~MaskBit();
	}
	
TBool CHCIServerRequestManager::IsAsyncBlocked()
	{
	LOG_FUNC
	
	return (iBlockMask & MaskBit());
	}
	
CHCIServerRequestManager::CHCIServerRequestManager(CCoreHCIPluginImpl& aHCI)
 :	iHCI(aHCI)
	{
	LOG_FUNC
	}
	
void CHCIServerRequestManager::ConstructL()
	{
	LOG_FUNC
	}

void CHCIServerRequestManager::ServiceL(CHCISession& aSession, const RMessage2& aMessage)
	{
	if(aMessage.Function() == EServiceSpecificRequest)
		{
		if(HasCapabilities(aMessage))
			{
			DoServiceL(aSession, aMessage);
			}
		// else HasCapabilities failed (and has dealt with aMessage).
		}
	else
		{
		// The client has issued a bad HCI Server request, we only expect a server specific
		// request to be made (the specific request being encoded in the TIpcArgs).
		PanicClient(aMessage, EUnknownHciServerFunction);
		}
	}

//
// HCI Direct Access Service
//
CHCIDirectAccessManager* CHCIDirectAccessManager::NewL(CCoreHCIPluginImpl& aHCI)
	{
	LOG_STATIC_FUNC
	
	CHCIDirectAccessManager* self = new (ELeave) CHCIDirectAccessManager(aHCI);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CHCIDirectAccessManager::SetHCICommandQueue(MHCICommandQueue& aHCICommandQueue)
	{
	LOG_FUNC
	
	__ASSERT_ALWAYS(!iHCICommandQueue, PANIC(KCoreHciPanicCat, EInterfaceAlreadyInitialised));
	iHCICommandQueue = &aHCICommandQueue;
	}

/*virtual*/ TBool CHCIDirectAccessManager::HasCapabilities(const RMessage2& aMessage)
	{
	LOG_FUNC
	
	TBool allowed = EFalse;
	
	switch (aMessage.Int0())
		{
	case KHCIDirectAccessVendorCommand:
		// Drop through
	case KHCIDirectAccessCancel: // requires Network Control + Local Services
		allowed = aMessage.HasCapability(ECapabilityNetworkControl) && aMessage.HasCapability(ECapabilityLocalServices);
		break;

	default:
		PanicClient(aMessage, EUnknownDirectAccessFunction);
		return EFalse;
		}
	
	if(!allowed)
		{
		aMessage.Complete(KErrPermissionDenied);
		}
		
	return allowed;
	}

/*virtual*/ void CHCIDirectAccessManager::DoServiceL(CHCISession& aSession, const RMessage2& aMessage)
	{
	LOG_FUNC
	
	TInt request = aMessage.Int0();

	switch (request)
		{
	case KHCIDirectAccessCancel:
		if (iAsyncTaskSession != &aSession)
			{
			return; //drop - not for current async task
			}

		// clear cache
		iMessage = NULL;
		
		// perform cancellation procedure
		CompleteAsyncTask(KErrCancel);
		break;

	case KHCIDirectAccessVendorCommand:
		{
		if (IsAsyncBlocked())
			{
			aMessage.Complete(KErrInUse);
			return;
			}

		const TInt len = User::LeaveIfError(aMessage.GetDesLength(1));
		
		// Check the Vendor Command is at least 3 bytes long for the opcode
		// and the length
		if (len < 3)
			{
			PanicClient(aMessage, EInvalidVendorCommand);
			break;
			}
		
		HBufC8* buf = HBufC8::NewLC(len);
		TPtr8 params(buf->Des());
		aMessage.ReadL(1, params);
		
		// Extract the opcode and create the new command
		TUint16 opcode = LittleEndian::Get16(params.Ptr());
		
		if (iHCICommandQueue)
			{
			CVendorDebugCommand* cmd = CVendorDebugCommand::NewL(opcode);

			// Remove the opcode and length from the Vendor Command parameters,
			// these will be added again to the command frame once it has been
			// formatted
			cmd->Command() = params.RightTPtr(params.Length() - 3);
			
			// cmd will be deleted in MhcqAddCommandL if Leaves.
			TUint qid = iHCICommandQueue->MhcqAddCommandL(cmd, *this);

			// Block only after we succeed in adding a command.
			BlockAsync(aSession, aMessage);

			// Cache to return completion data to client
			iMessage = &aMessage;
			}
		else
			{
			// Slight possible race that a request is made before we have been
			// given the command queue to work with - so don't panic just error.
			aMessage.Complete(KErrNotReady);
			}
		CleanupStack::PopAndDestroy(buf);
		}
		break;

	default:
		// Unknown message, panic client
		PanicClient(aMessage, EUnknownDirectAccessFunction);
		break;
		}
	}

/*virtual*/ void CHCIDirectAccessManager::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* /*aCommand*/)
	{
	LOG_FUNC

	// clear cache
	iMessage = NULL;

	CompleteAsyncTask(aErrorCode);
	}

/*virtual*/ void CHCIDirectAccessManager::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aCommand)
	{
	LOG_FUNC

	// We are not the unmatched event observer therefore we must always get a
	// command
	__ASSERT_DEBUG(aCommand, PANIC(KCoreHciPanicCat, EReceivedUnmatchedEvent));
	
	// It is possible to receive an event but not have a message to complete. This
	// could happen if a request was cancelled after a Vendor Specific command was
	// sent, in this case we do nothing.
	if (iMessage)
		{
		TInt err = KErrNone;
		
		const THCIEventCode eventcode(aEvent.EventCode());
		switch(eventcode)
			{
		case ECommandCompleteEvent:
			{
			if ((aCommand->Opcode() & KVendorDebugOGF) == KVendorDebugOGF)
				{
				const TVendorDebugCompleteEvent& event = TVendorDebugCompleteEvent::Cast(aEvent);
				
				// Return the event
				err = iMessage->Write(KHCIServerIpcDesOutParam, event.VendorDebugData());
				}
			else
				{
				__ASSERT_DEBUG(EFalse, PANIC(KHciServerPanicCat, ENonVendorOpcodeCompleted));
				}
			}
			break;

		case EVendorDebugEvent:
			{
			const TVendorDebugEvent& event = TVendorDebugEvent::Cast(aEvent);

			// Return the event
			err = iMessage->Write(KHCIServerIpcDesOutParam, event.VendorDebugData());
			}
			break;
			
		default:
			__ASSERT_DEBUG(EFalse, PANIC(KCoreHciPanicCat, EReceivedUnknownEvent));
			break;
			}

		CompleteAsyncTask(err);
		}
	}

/*virtual*/ void CHCIDirectAccessManager::CompleteAsyncTask(TInt aError)
	{
	LOG_FUNC

	if (!IsAsyncBlocked())
		{
		return; //drop
		}

	CHCIServerRequestManager::CompleteAsyncTask(aError);
	}

CHCIDirectAccessManager::CHCIDirectAccessManager(CCoreHCIPluginImpl& aHCI)
:	CHCIServerRequestManager(aHCI)
	{
	LOG_FUNC
	}
	

TUint CHCIDirectAccessManager::MaskBit() const
	{
	LOG_FUNC
	
	return EHCIDirectAccessMaskBit;
	}
			
//
// Power Control Service
//
CBTPowerControlManager* CBTPowerControlManager::NewL(CCoreHCIPluginImpl& aHCI)
	{
	LOG_STATIC_FUNC
	
	CBTPowerControlManager* self = new (ELeave) CBTPowerControlManager(aHCI);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CBTPowerControlManager::HasCapabilities(const RMessage2& aMessage)
	{
	LOG_FUNC
	
	TBool allowed(EFalse);
	
	switch (aMessage.Int0())
		{
	case KHCICancelPower:
		// fall through
	case EHCISetPower: // requires Network Control + Local Services
		allowed = aMessage.HasCapability(ECapabilityNetworkControl) && aMessage.HasCapability(ECapabilityLocalServices);
		break;
	
	case EHCIGetPower: // requires Local Services
		allowed = aMessage.HasCapability(ECapabilityLocalServices);
		break;

	default:
		PanicClient(aMessage, EUnknownPowerControlFunction);
		return EFalse;
		}
		
	if(!allowed)
		{
		aMessage.Complete(KErrPermissionDenied);
		}
		
	return allowed;
	}

/*virtual*/ void CBTPowerControlManager::DoServiceL(CHCISession& aSession, const RMessage2& aMessage)
	{
	LOG_FUNC

	TInt request = aMessage.Int0();
	TInt err;
	TBTPowerState state;
	
	switch (request)
		{
	case KHCICancelPower:
		if(iAsyncTaskSession!=&aSession)
			{
			return; //drop - not for current async task
			}

		//perform cancellation procedure
		CompleteAsyncTask(KErrCancel);
		break;

	case EHCISetPower:
		if(IsAsyncBlocked())
			{
			aMessage.Complete(KErrInUse);
			return;
			}
		
		//Should be called before set power because it could complete synchronously
		BlockAsync(aSession, aMessage);		
		state = static_cast<TBTPowerState>(aMessage.Int3());
		err = iHCI.SetPower(state, NULL);
		if(err != KErrNone)
			{
			aMessage.Complete(err);
			return;
			}
		break;

	case EHCIGetPower:
		{
		err = iHCI.GetPower(state, NULL);
		if(err != KErrNone)
			{
			aMessage.Complete(err);
			return;
			}
		
		TPckg<TBTPowerState> pckg(state);
		TRAP(err, aMessage.WriteL(EPSStateReturnSlot, pckg));
		aMessage.Complete(err);
		}
		break; //synchronous so do not block
	
	default:
		// Unknown message
		PanicClient(aMessage, EUnknownPowerControlFunction);
		break;
		}
	}

/*virtual*/ void CBTPowerControlManager::CompleteAsyncTask(TInt aError)
	{
	LOG_FUNC
	
	if(!IsAsyncBlocked())
		{
		return; //drop
		}

	CHCIServerRequestManager::CompleteAsyncTask(aError);
	}

void CBTPowerControlManager::CompleteRequest(TInt aError)
	{
	CompleteAsyncTask(aError);
	}

CBTPowerControlManager::CBTPowerControlManager(CCoreHCIPluginImpl& aHCI)
:	CHCIServerRequestManager(aHCI)
	{
	LOG_FUNC
	}
	

TUint CBTPowerControlManager::MaskBit() const
	{
	LOG_FUNC
	
	return EBTPowerControlMaskBit;
	}

//
// Bluetooth HCI Utils Service
//
CBluetoothDutModeManager* CBluetoothDutModeManager::NewL(CCoreHCIPluginImpl& aHCI)
	{
	LOG_STATIC_FUNC
	
	CBluetoothDutModeManager* self = new (ELeave) CBluetoothDutModeManager(aHCI);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CBluetoothDutModeManager::HasCapabilities(const RMessage2& aMessage)
	{
	LOG_FUNC
	
	TBool allowed(EFalse);
	
	switch (aMessage.Int0())
		{
	case RBluetoothDutMode::EHCIActivateDutMode:
		// fall through
	case RBluetoothDutMode::EHCIDeactivateDutMode: // requires Network Control + Local Services
		allowed = aMessage.HasCapability(ECapabilityNetworkControl) && aMessage.HasCapability(ECapabilityLocalServices);
		break;

	default:
		PanicClient(aMessage, EUnknownBluetoothDutModeFunction);
		return EFalse;
		}
		
	if(!allowed)
		{
		aMessage.Complete(KErrPermissionDenied);
		}
		
	return allowed;
	}

/*virtual*/ void CBluetoothDutModeManager::DoServiceL(CHCISession& /*aSession*/, const RMessage2& aMessage)
	{
	LOG_FUNC

	TInt request = aMessage.Int0();
	TInt err = KErrNotSupported;

	switch (request)
		{
	case RBluetoothDutMode::EHCIActivateDutMode:
		// Implementation is adaptation specific and therefore not supported in our reference HCI Server
		LOG(_L("EHCIActivateDutMode"));
		aMessage.Complete(err);
		break;

	case RBluetoothDutMode::EHCIDeactivateDutMode:
		// Implementation is adaptation specific and therefore not supported in our reference HCI Server
		LOG(_L("EHCIDeactivateDutMode"));
		aMessage.Complete(err);
		break;

	default:
		// Unknown message
		PanicClient(aMessage, EUnknownBluetoothDutModeFunction);
		break;
		}
	}

CBluetoothDutModeManager::CBluetoothDutModeManager(CCoreHCIPluginImpl& aHCI)
:	CHCIServerRequestManager(aHCI)
	{
	LOG_FUNC
	}
	

TUint CBluetoothDutModeManager::MaskBit() const
	{
	LOG_FUNC
	
	return EBluetoothDutModeMaskBit;
	}

//
// A2DP Optimiser Service
//
CA2dpOptimiserManager* CA2dpOptimiserManager::NewL(CCoreHCIPluginImpl& aHCI)
	{
	LOG_STATIC_FUNC
	
	CA2dpOptimiserManager* self = new (ELeave) CA2dpOptimiserManager(aHCI);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CA2dpOptimiserManager::HasCapabilities(const RMessage2& aMessage)
	{
	LOG_FUNC
	
	TBool allowed(EFalse);
	
	switch (aMessage.Int0())
		{
	case RA2dpOptimiser::EHCIA2dpOptimiseAcl:
		// fall through
	case RA2dpOptimiser::EHCIA2dpRemoveAclOptimisation: // requires Network Control + Local Services
		allowed = aMessage.HasCapability(ECapabilityNetworkControl) && aMessage.HasCapability(ECapabilityLocalServices);
		break;

	default:
		PanicClient(aMessage, EUnknownA2dpOptimiserFunction);
		return EFalse;
		}
		
	if(!allowed)
		{
		aMessage.Complete(KErrPermissionDenied);
		}
		
	return allowed;
	}

/*virtual*/ void CA2dpOptimiserManager::DoServiceL(CHCISession& /*aSession*/, const RMessage2& aMessage)
	{
	LOG_FUNC

	TInt request = aMessage.Int0();
	TInt err = KErrNotSupported;
	TBuf<20> targetAddrBuf;
	
	switch (request)
		{
	case RA2dpOptimiser::EHCIA2dpOptimiseAcl:
		// Implementation is adaptation specific and therefore not supported in our reference HCI Server
		if (aMessage.GetDesLength(1) == sizeof(TA2dpOptimisationParams))
			{
			TPckgBuf<TA2dpOptimisationParams> args;
	
			aMessage.ReadL(1, args);
			args().RemoteDeviceAddress().GetReadable(targetAddrBuf);
			LOG3(_L("EHCIA2dpOptimiseAcl BT Addr: %S, Peak Bandwidth: %d, Access Latency: %d\n"), 
						&targetAddrBuf, args().PeakBandwidth(), args().AccessLatency());
	
			aMessage.Complete(err);
			}
		else
			{
			PanicClient(aMessage, EInvalidArgument);
			}
		break;

	case RA2dpOptimiser::EHCIA2dpRemoveAclOptimisation:
		// Implementation is adaptation specific and therefore not supported in our reference HCI Server
		if (aMessage.GetDesLength(1) == sizeof(TBTDevAddr))
			{
			TPckgBuf<TBTDevAddr> addrBuf;
	
			aMessage.ReadL(1, addrBuf);
			addrBuf().GetReadable(targetAddrBuf);
			LOG1(_L("EHCIA2dpRemoveAclOptimisation BT Addr: %S\n"), &targetAddrBuf);
	
			aMessage.Complete(err);
			}
		else
			{
			PanicClient(aMessage, EInvalidArgument);
			}
		break;

	default:
		// Unknown message
		PanicClient(aMessage, EUnknownA2dpOptimiserFunction);
		break;
		}
	}

CA2dpOptimiserManager::CA2dpOptimiserManager(CCoreHCIPluginImpl& aHCI)
:	CHCIServerRequestManager(aHCI)
	{
	LOG_FUNC
	}
	

TUint CA2dpOptimiserManager::MaskBit() const
	{
	LOG_FUNC
	
	return EA2dpOptimiserMaskBit;
	}

