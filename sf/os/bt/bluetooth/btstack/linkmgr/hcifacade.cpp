// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// The conduit to the HCI and HW - formulates and Qs commands, and implements HCI events
// 
//

#include <bluetooth/logger.h>
#include "hcifacade.h"
#include "linkmgr.h"
#include "hostresolver.h"
#include "linkutil.h"

#include "AclDataQController.h"
#include "linkmuxer.h"
#include "linkconsts.h"

#include <e32std.h>
#include <e32cmn.h>
#include <bt_sock.h>
#include <es_ini.h>
#include <e32base.h>

#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hcicommandqitem.h>
#include <bluetooth/hcicmdqcontroller.h>

#include <bluetooth/hci/hciopcodes.h>

#include <bluetooth/hci/controllerinitialisationinterface.h>
#include <bluetooth/hci/hcidataframer.h>
#include <bluetooth/hci/readlocalsupportedfeaturescommand.h>
#include <bluetooth/hci/resetcommand.h>
#include <bluetooth/hci/readbdaddrcommand.h>
#include <bluetooth/hci/readlocalversioninfocommand.h>
#include <bluetooth/hci/writeconnectionaccepttimeoutcommand.h>
#include <bluetooth/hci/writevoicesettingcommand.h>
#include <bluetooth/hci/readclassofdevicecommand.h>
#include <bluetooth/hci/readbuffersizecommand.h>
#include <bluetooth/hci/setcontrollertohostflowcontrolcommand.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCI_FACADE);
#endif

// definitions
const TBasebandTime TBasebandPolicy::KPageTimeoutR0 = static_cast<TBasebandTime>(2.56 /*seconds*/ / KBasebandSlotTime); // in baseband slots
const TBasebandTime TBasebandPolicy::KPageTimeoutR1 = static_cast<TBasebandTime>(5.12 /*seconds*/ / KBasebandSlotTime); // in baseband slots
const TBasebandTime TBasebandPolicy::KPageTimeoutR2 = static_cast<TBasebandTime>(10.24 /*seconds*/ / KBasebandSlotTime); // in baseband slots
const TReal TBasebandPolicy::KBestEffortTimeMultiplier = 2;
const TReal TBasebandPolicy::KQuickTimeMultiplier = 0.5;

_LIT(KHciUtilComponentName, "bluetooth_stack");

CHCIFacade* CHCIFacade::NewL(CLinkMgrProtocol& aProtocol)
	{
	LOG_STATIC_FUNC
	CHCIFacade* p= new (ELeave) CHCIFacade(aProtocol);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CHCIFacade::ConstructL()
/**
   2nd phase construct the HCI Facade.
   
   This is where the stack loads both the HCI CommandQ and the CoreHCI and
   then distributes the required APIs between the different components.
**/
	{
	LOG_FUNC
	// Create HCI CommandQ
	iCmdController = CHCICmdQController::NewL();

	// Create HCI Utility library
	iHciUtil = CHciUtil::NewL(KHciUtilComponentName);

	// If we can't open the ini file then this will be treated in the same way
	// as not reading a valid UID from the ini file.
	TRAP_IGNORE(iHciUtil->OpenIniFileL());
	
	// Create Core HCI plugin
	_LIT(KSection, "CoreHci");
	_LIT(KCoreHciUidTag, "EcomUid");

	TUid coreHciImplUid = TUid::Null();
	TRAPD(err, coreHciImplUid = iHciUtil->GetUidFromFileL(KSection, KCoreHciUidTag));
	
	if (err == KErrNone)
		{
		// Valid UID found, load it
		iCoreHciPlugin = CCoreHCIPlugin::NewL(coreHciImplUid);
		}
	else
		{
		// No UID found in ini file, attempt to load single instance of 
		// implementation
		iCoreHciPlugin = CCoreHCIPlugin::NewL();
		}
	
	// Get Core HCI APIs
	iCoreHci = static_cast<MCoreHci*>(iCoreHciPlugin->Interface(TUid::Uid(KCoreHciInterfaceUid)));
	iHctl = static_cast<MHCTLInterface*>(iCoreHciPlugin->Interface(TUid::Uid(KHCTLInterfaceUid)));
	
	iControllerInitialisor = static_cast<MControllerInitialisationInterface*>
								(iCoreHciPlugin->Interface(TUid::Uid(KControllerInitialisationInterfaceUid)));
	if (iControllerInitialisor != NULL)
		{
		// attempt to get the Controller Initialisation Abort Extension API
		iControllerInitAbortExtension = static_cast<MControllerInitialisationAbortExtensionInterface*>
								(iCoreHciPlugin->Interface(TUid::Uid(KControllerInitialisationAbortExtenstionInterfaceUid)));
		}
	
	iDataFramer = static_cast<MHCIDataFramer*>(iCoreHciPlugin->Interface(TUid::Uid(KHCIDataFramerInterfaceUid)));
	iHardResetInitiator = static_cast<MHardResetInitiator*>(iCoreHciPlugin->Interface(TUid::Uid(KHCHardResetUid)));

	// Panic if we don't get the required interfaces, note that the initialisor
	// interface is optional
	__ASSERT_ALWAYS(iCoreHci && iHctl && iDataFramer && iHardResetInitiator,
					Panic(EHCIIncompleteInterfaces));
	
	// Provide direct APIs
	iCmdController->SetLinkMuxNotifier(*this);
	iCmdController->SetHCIUnmatchedEventObserver(*this);
	iCmdController->SetHardResetInitiator(*this);
	iCmdController->SetPhysicalLinksState(*this);

	iCoreHci->MchSetHardResetInitiator(*this);
	iCoreHci->MchSetPhysicalLinksState(*this);	
	iCoreHci->MchSetDataEventObserver(*this);
	iCoreHci->MchSetDataObserver(*this);
	iCoreHci->MchSetChannelObserver(*this);
	iCoreHci->MchSetControllerStateObserver(*this);
	
	// Provide Core HCI with CommandQ components
	iCoreHci->MchSetCommandEventObserver(*(static_cast<MHCICommandEventObserver*>(iCmdController)));
	iCoreHci->MchSetHCICommandQueue(*(static_cast<MHCICommandQueue*>(iCmdController)));
	
	// Provide CommandQ with HCI components
	iCmdController->SetHCTLInterface(*iHctl);
	iCmdController->SetHCICommandAllocator(*(static_cast<MHCICommandAllocator*>(iCoreHciPlugin->Interface(TUid::Uid(KHCICommandAllocatorInterfaceUid)))));

	// Initialise initialisor plugin if present
	if (iControllerInitialisor != NULL)
		{
		// Provide Initialisor with Stack
		iControllerInitialisor->MciiSetControllerInitialisationObserver(*this);

		// Provide Initialisor with CommandQ
		iControllerInitialisor->MciiSetHCICommandQueue(*(static_cast<MHCICommandQueue*>(iCmdController)));
		}

	// Attempt to supply the HCI with the Client Usage API
	MHCIClientUsageCallback* hciClientUsageCallback = static_cast<MHCIClientUsageCallback*>
								(iCoreHciPlugin->Interface(TUid::Uid(KHCIClientUsageCallbackUid)));
	
	if (hciClientUsageCallback)
		{
		// Support for the Client Usage Callback API is optional for CoreHCI plugins, pass the HCI
		// the Client Usage API if supported.
		hciClientUsageCallback->MhcucSetClientUsage(*this);
		}

	iAFHTimer=CAFHTimer::NewL(*this);
	
	iLastPowerState = EBTOn;
	
	// used later to ensure that we have enough data to call SetEventMask
	iReadLocalSupportedFeaturesComplete = EFalse;
	iReadLocalVersionComplete           = EFalse;
	}
	
void CHCIFacade::SetLinkMuxer(CLinkMuxer& aLinkMuxer)
	{
	LOG_FUNC
	iLinkMuxer=&aLinkMuxer;
	}


void CHCIFacade::SetPhysicalLinksMgr(CPhysicalLinksManager& aLinksMgr)
	{
	LOG_FUNC
	iLinksMgr = &aLinksMgr;
	}

TInt CHCIFacade::SendInitialisationCommand(CHCICommandBase* aCommand)
	/*
	  This method takes ownership of the command.
	*/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aCommand != NULL, Panic(EHCICommandNullItem));
	__ASSERT_ALWAYS(iInitState==EResetting || iInitState==EPostReset, Panic(EHCICtrlrInitAddingInitialisationCommandInBadState));
	if(iInitState==EResetting)
		{
		__ASSERT_ALWAYS(iOutstandingCommands.Count()==0, Panic(EHCICtrlrInitOnlyOneResetCmdAllowed));
		}

	TInt err = KErrNone;
	TUint qid = 0;

	// Ownership of the command is passed to the queue.
	// MhcqAddInitCommandL guarantees to take ownership even if it
	// leaves.

	// see CHCIFacade::MhcqcCommandEventReceived for completion
	TRAP(err, qid = iCmdController->MhcqAddInitCommandL(aCommand, *this));
	if (err != KErrNone)
		{
		iInitialisationError = ETrue;

		LOG1(_L("Error! CHCIFacade::SendInitialisationCommand %d"), err);
		return err;
		}

	// We need to know when we have completed initialisation of the stack so that
	// we can Start() the HCI Command Queue. To do this we keep a list of all the
	// initialisation command opcodes and then remove them from the list when we
	// get the corresponding completion event. The stack initialisation is complete
	// when the list of opcodes is empty.
	if ((err = AddOutstandingCommandOpCode(aCommand->Opcode()))	!= KErrNone)
		{
		if (iCmdController->MhcqRemoveCommand(qid, *this) != KErrNone)
			{
			Panic(EHCICtrlrInitFailedToRemoveCmd);
			}
		iInitialisationError = ETrue;

		LOG1(_L("Error! CHCIFacade::SendInitialisationCommand %d"), err);
		return err;
		}

	return KErrNone;
	}

void CHCIFacade::InitL(const TBTLocalDevice& aDeviceConfig)
/**
	The start-up strategy

	If there is an Initialisor the pre-reset initialisation method will
	be called on it at this point. When the Initialisor pre-reset method
	completes the stack will be called back to enable it to send the HCI
	reset command, see McioPreResetCommandComplete().
	
	If there is no Initialisor then this method will call the pre-reset
	complete method directly allowing the stack to send the HCI reset
	command.
**/
	{
	LOG_FUNC
	// Only re-initialise the stack is the current state is not pre-reset.
	if(iInitState != EPreReset)
		{
		// Initialise the Command Queue
		iCmdController->Initialise();
		iInitState = EPreReset;
		
		// DeviceClass is cached and retrieved from HCI if not currently
		// set in aDeviceConfig
		iDeviceClass = aDeviceConfig.DeviceClass();

		// Get the Local Name from the Persist storage
		TInt err(iLinkMgrProtocol.SetLocalDeviceName(aDeviceConfig.DeviceName()));
		if (KErrNone != err && KErrNotSupported != err)
			{
			User::Leave(err);
			}
		if (iControllerInitialisor != NULL)
			{
			// see CHCIFacade::McioPreResetCommandComplete
			iControllerInitialisor->MciiPreResetCommand();
			}
		else
			{
			// There is no initialisation plugin so behave as if there
			// were and it had completed its pre reset phase
			McioPreResetCommandComplete(KErrNone);
			}
		}
	}

CHCIFacade::CHCIFacade(CLinkMgrProtocol& aProtocol)
  : iLinkMgrProtocol(aProtocol),
    iInitState(EIdle),
    iInitialisationError(EFalse)
	{
	LOG_FUNC
	}

CHCIFacade::~CHCIFacade()
	{
	LOG_FUNC
	delete iAFHTimer;

	delete iCoreHciPlugin;
	
	if(iCmdController)
		{
		iCmdController->MhcqRemoveAllCommands(*this);
		delete iCmdController;
		}
		
	iOutstandingCommands.Close();
	delete iHciUtil;
	}

CHctlAclDataFrame* CHCIFacade::NewACLDataFrameL(TUint16 aSize) const
	{
	LOG_FUNC
	return (iDataFramer->MhdfNewAclDataFrameL(aSize));
	}

#ifdef STACK_SCO_DATA
CHctlSynchronousDataFrame* CHCIFacade::NewSynchronousDataFrameL(TUint8 aSCODataLen) const
	{
	LOG_FUNC
	return (iDataFramer->MhdfNewSynchronousDataFrameL(aSCODataLen));
	}
#endif

THCITransportChannel CHCIFacade::HCTLState() const
	{
	LOG_FUNC
	return iHCTLState; // give to muxer
	}



void CHCIFacade::HandlePowerStatusChange(TBTPowerState aStatus)
/**
	This method assumes that the power is actually changing, i.e. from off to on or
	from on to off, and it is the responsibility of the caller, the hctl in non-error
	conditions, to ensure this.
	
	The only real dangerous transition is from on to on as this would intialise the
	stack even if it is currently in an on state with or without existing state and 
	connections.
**/
	{
	LOG_FUNC
	switch(aStatus)
		{
	case EBTOn:
		{
		// Start-up Bluetooth
		// Avoid from ON to ON
		__ASSERT_DEBUG (iLastPowerState == EBTOff, Panic(EHCIPowerStateError));
		
		iLastPowerState = aStatus;
		//recovery the channels
		iLinkMgrProtocol.LinkMuxer().ChannelsFree(iHCTLState);
		
		TBTLocalDevice settings(iLinkMgrProtocol.LocalDevice());
		iLinkMgrProtocol.DesiredLocalDeviceSettings().Modify(settings);
		
		TRAPD(err, InitL(settings));
		// Hopefully this should just work it won't rename the device though 
		// since that is persisted
		if (err)
			{
			HandlePowerStatusChange(EBTOff);
			return;
			}
		else
			{
			iLinkMgrProtocol.DesiredLocalDeviceSettings().ResetChangesMask();
			// Reset the inquiry manager
			iLinkMgrProtocol.InquiryMgr().SetHWState(CBTInquiryMgr::EIdle);
			// Clear debug mode
			iLinkMgrProtocol.SecMan().ClearDebugMode();
			}
		}
		break;
		
	case EBTOff:
		{
		// Reset the Command Queue
		// Avoid from OFF to OFF
		__ASSERT_DEBUG (iLastPowerState == EBTOn, Panic(EHCIPowerStateError));
			
		iLastPowerState = aStatus;
		
		// Determine if we are waiting for a callback from the initialisation plugin
		if ((iInitState == EPreReset) || (iInitState == EReset))
			{
			// cancel initialisation callback if possible
			if (iControllerInitAbortExtension != NULL)
				{
				iControllerInitAbortExtension->MciaeiAbortInitialisation();
				}
			}
		iInitState = EIdle;
			
		iCmdController->Reset();
		iOutstandingCommands.Reset();
		
		// Ensure that no command or data messages are output by blocking all channels
		//
		iLinkMgrProtocol.LinkMuxer().ChannelsClosed(KHCITransportAllChannels);
		iLinkMgrProtocol.Error(KErrHardwareNotAvailable);
		// Reset UI
		//
		iLinkMgrProtocol.SetUIConnecting(EFalse);
		iLinkMgrProtocol.SetUINumPhysicalLinks(0);
		// The h/w has been (or will be) reset, so we need to clear our locally cached data
		//
		iLinkMgrProtocol.LinkMuxer().ResetFlowControlMode();
		iLinkMgrProtocol.ClearPendingLocalDeviceSettingsCod();
		
		// Removes any pending AFH Channel Classification command 
		// and cancels timer.
		// NB This ensures AFH host channel classification command blocking is 
		// not in place if power comes back on.
		iAFHTimer->Reset();
		}
		break;
		
	default:
		{
		Panic(EHCIUnknownPowerState);
		}
		break;
		}
	
	// Successfully changed power state
	iLinkMgrProtocol.UpdateLocalDevicePower(aStatus);
	}

TInt CHCIFacade::ChangeMode(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	TRAPD(err, DoChangeModeL(aMode, aConnHandle));
	return err;
	}

void CHCIFacade::DoChangeModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	switch (aMode)
		{
		case ESniffMode:
			{
			SniffL(aConnHandle);
			break;
			}
		case EParkMode:
			{
			ParkL(aConnHandle);
			break;
			}
		case EHoldMode:
			{
			HoldL(aConnHandle);
			break;
			}
		case EScatterMode:
		case EActiveMode:
			__ASSERT_DEBUG(0, Panic(EHCICommandBadArgument));
			break;
		}
	}
	
TInt CHCIFacade::ExitMode(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	TRAPD(err, DoExitModeL(aMode, aConnHandle));
	return err;
	}
	
void CHCIFacade::DoExitModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	switch (aMode)
		{
		case ESniffMode:
			{
			ExitSniffL(aConnHandle);
			break;
			}
		case EParkMode:
			{
			ExitParkL(aConnHandle);
			break;
			}
		// Not possile to prematurely exit hold mode
		default:
			{
			break;
			}
		}
	}

void CHCIFacade::SetupFlowControlL(TFlowControlMode aMode)
/** 
	Set up flow control scheme to be used.
	SetupFlowControlL should only be called once on init because it may leave.

	If TFlowControlMode is chosen to be EFlowControlFromHostControllerOnly or 
	ETwoWayFlowControlEnabled, then it is the responsibility of the HCI client
	to issue the appropriate HostBufferSize(..) command.
*/
	{
	LOG_FUNC
	
	switch (aMode)
		{
	case ENoFlowControl:
	case EFlowControlToHostControllerOnly:
	case EFlowControlFromHostControllerOnly:
	case ETwoWayFlowControlEnabled:
		// a valid argument has been provided
		break;
	default:
		Panic(ELinkMgrNoSuchFlowControlMode);
		break;
		}
	
	TBool ctrlerToHostFlowControl = (aMode == ETwoWayFlowControlEnabled) || (aMode == EFlowControlFromHostControllerOnly);
	TBool hostToCtrlerFlowControl = (aMode == ETwoWayFlowControlEnabled) || (aMode == EFlowControlToHostControllerOnly);
	
	if(hostToCtrlerFlowControl)
		{
		LEAVEIFERRORL(SendInitialisationCommand(CReadBufferSizeCommand::NewL()));
		}
	
	if(ctrlerToHostFlowControl)
		{
		static const TUint8 KControllerToHostFlowControlAclOnSyncOff = 0x01; 
		CHCICommandBase* command = 
			CSetControllerToHostFlowControlCommand::NewL(KControllerToHostFlowControlAclOnSyncOff);
		LEAVEIFERRORL(SendInitialisationCommand(command));
		// When this command successfully completes then host buffer size command will be issued.
		}
	else
		{
		iLinkMuxer->RecordHostControllerToHostFlowControl(EFalse);
		}
	}


// simple forwarding functions so that all stack usage to HCI is via Facade.

TInt CHCIFacade::WriteACLData(const CHctlAclDataFrame& aFrame) const
	{
	LOG_FUNC
	return iHctl->MhiWriteAclData(aFrame.HCTLPayload());
	}

TInt CHCIFacade::WriteSCOData(const CHctlSynchronousDataFrame& aFrame) const
	{
	LOG_FUNC
	return iHctl->MhiWriteSynchronousData(aFrame.HCTLPayload());
	}

void CHCIFacade::FormatACLData(CHctlAclDataFrame& aFrame, THCIConnHandle aHandle,
							   TUint8 aOptions, const TDesC8& aData) const
	{
	LOG_FUNC
	TUint8 flags = aOptions;
	TAclPacketBoundaryFlag pbFlag = static_cast<TAclPacketBoundaryFlag>(flags & KPacketPBFlagMask);
	TAclPacketBroadcastFlag bcFlag = static_cast<TAclPacketBroadcastFlag>((flags & KPacketBCFlagMask) >> KPacketPBtoBCFlagShift);
	iDataFramer->MhdfFormatAclData(aFrame, aHandle, pbFlag, bcFlag, aData);
	}

void CHCIFacade::FormatSCOData(CHctlSynchronousDataFrame& aFrame, THCIConnHandle aHandle,
							   const TDesC8& aData) const
	{
	LOG_FUNC
	iDataFramer->MhdfFormatSynchronousData(aFrame, aHandle, aData);
	}

void CHCIFacade::MhriStartHardReset()
	{
	LOG_FUNC
	/*
	Could have forced a...
	HandlePowerStatusChange(EBTOff); 
	here - but instead let HCTL call this.
	*/
	iHardResetInitiator->MhriStartHardReset();
	}

TUint16 CHCIFacade::ReadACLReportingInterval() const
	{
	LOG_FUNC
	return 0;
	}

TUint16 CHCIFacade::ReadACLFramingOverhead() const
	{
	LOG_FUNC
	return 0;
	}

// MControllerInitialisationObserver
void CHCIFacade::McioPreResetCommandComplete(TInt aError)
	{
	LOG_FUNC
	CHCICommandBase* command(NULL);
	TInt err;

	// check that we are in the correct state
	__ASSERT_ALWAYS(iInitState == EPreReset, Panic(EIncorrectStateOnPreResetCallback));
	iInitState = EResetting;
	
	if (aError != KErrNone)
		{
		// The initialisor plugin has failed pre reset initialisation
		err = aError;
		}
	else
		{
		// Send Reset command
		TRAP(err, command = CResetCommand::NewL());
		
		if (err == KErrNone)
			{
			err = SendInitialisationCommand(command);
			}
		}
		
	if (err != KErrNone)
		{
		// Initialisation has failed. At this point all we can do is power down
		// Bluetooth. This means that initialisation can be attempted again by
		// turning on the power but this again could fail.
		HandlePowerStatusChange(EBTOff);
		}
	}

void CHCIFacade::DoSendPostResetCommandsL()
	{
	LOG_FUNC
	User::LeaveIfError(SendInitialisationCommand(CReadLocalSupportedFeaturesCommand::NewL()));
	User::LeaveIfError(SendInitialisationCommand(CReadBdaddrCommand::NewL()));
	User::LeaveIfError(SendInitialisationCommand(CReadLocalVersionInfoCommand::NewL()));
	
#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL
#pragma message("Transport FlowControl = Two Way")

	SetupFlowControlL(ETwoWayFlowControlEnabled);
		
#else
#pragma message("Transport FlowControl = None from HostController")

	SetupFlowControlL(EFlowControlToHostControllerOnly);
		
#endif

	User::LeaveIfError(SendInitialisationCommand(CWriteConnectionAcceptTimeoutCommand::NewL(KHCIDefaultAcceptTimeout)));
	User::LeaveIfError(SendInitialisationCommand(CWriteVoiceSettingCommand::NewL(KBTVoiceSetting)));

	if (iDeviceClass)
		{
		// Some phones never set their CoD in s/w, we don't want to
		// overwrite their h/w settings
		LOG1(_L("DoSendPostResetCommandsL - SetDeviceClassL (0x%x)"), iDeviceClass);
		iLinkMgrProtocol.SetDeviceClassL(iDeviceClass);
		}
	else
		{
		// We want to update the P&S value with the h/w value
		User::LeaveIfError(SendInitialisationCommand(CReadClassOfDeviceCommand::NewL()));
		}

	TRAPD(err, iLinkMgrProtocol.SetInquiryAndPageScanningL());
	if(err != KErrNone && err != KErrNotSupported)
		{
		User::Leave(err);
		}
	}
	
void CHCIFacade::McioPostResetCommandComplete(TInt aError)
	{
	LOG_FUNC
	TInt err;
	
	// check that we are in the correct state
	__ASSERT_ALWAYS(iInitState == EReset, Panic(EIncorrectStateOnPostResetCallback));
	iInitState = EPostReset;

	if (aError == KErrNone)
		{
		// Send post reset commands required by stack
		TRAP(err, DoSendPostResetCommandsL());
		}
	else
		{
		// The initialisor plugin has failed post reset initialisation
		err = aError;
		}
		
	if (err != KErrNone)
		{
		// Initialisation has failed. At this point all we can do is power down
		// Bluetooth. This means that initialisation can be attempted again by
		// turning on the power but this again could fail.
		HandlePowerStatusChange(EBTOff);
		}
	}

// MHCIDataObserver
void CHCIFacade::MhdoProcessAclData(THCIConnHandle aConnH, TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, const TDesC8& aData)
	{
	LOG_FUNC
	// Stack currently works with the old flag passing mechanism.  So recombine into the old
	// flag type before continuing.
	TUint8 flag = (aBoundaryFlag | (aBroadcastFlag << KPacketPBtoBCFlagShift)) << KPacketPBBCFlagShift;

	LOG3(_L("Link [HCIFacade_Events.cpp]: CHCIFacade data received on handle %d, flags %d, length %d"), aConnH, flag, aData.Length());

	iLinksMgr->ACLDataReceived(aConnH, flag, aData);
	}

void CHCIFacade::MhdoProcessSynchronousData(THCIConnHandle aConnH, TUint8 /*aReserved*/, const TDesC8& aData)
	{
	LOG_FUNC
	iLinksMgr->SCODataReceived(aConnH, aData);
	}

// MHCTLChannelObserver
void CHCIFacade::MhcoChannelOpen(THCITransportChannel aChannels)
/**
	The HCI has notified us that transport channels have become free
**/
	{
	LOG_FUNC
	LOG1(_L("Transport channels 0x%04x now free"), aChannels);

	// record the channel status
	iHCTLState |= aChannels;
	
	if (iLinkMuxer)
		{
		iLinkMuxer->ChannelsFree(aChannels); //try to send on this channel
		}
	}
	
void CHCIFacade::MhcoChannelClosed(THCITransportChannel aChannels)
	{
	LOG_FUNC
	// record the channel status
	iHCTLState &= (~aChannels) & KHCITransportAllChannels;
	
	// Tell the Muxer these channels are closed, if Muxer is ready
	if (iLinkMuxer)
		{
		iLinkMuxer->ChannelsClosed(aChannels);
		}
	}

// MControllerStateObserver
void CHCIFacade::McsoProcessPowerChange(TInt aError, TControllerChangeType aChangeType, TBTPowerState aState)
	{
	LOG_FUNC
	if(aError!=KErrNone)
		//Don't do anything - assume error implies no change took place
		{
		return;
		}
		
	if(aChangeType==MControllerStateObserver::EBTNonFatalChange)
		//Don't do anything - controller change will not affect us
		{
		return;
		}
		
	// For now continue to use HandlePowerStatusChange as common code for power states and hard reset states
	HandlePowerStatusChange(aState);
	}

void CHCIFacade::McsoProcessHardResetPhaseChange(TInt aError, TControllerChangeType aChangeType, TBTHardResetState aState)
	{
	LOG_FUNC
	if(aError!=KErrNone)
		{
		//We may need to inform user somehow that reset has failed - depends on
		//nature of error. What if error is KErrInUse? Should we set a timer
		//and try again?
		return;
		}
		
	if(aChangeType==MControllerStateObserver::EBTNonFatalChange)
		//Don't do anything - controller change will not affect us
		{
		return;
		}
		
	switch(aState)
		{
		case EBTResetStarted:
			HandlePowerStatusChange(EBTOff);
			break;
		case EBTResetComplete:
			HandlePowerStatusChange(EBTOn);
			break;
		default:
			Panic(EHCIUnknownHardResetState);
			break;
		}
	}

// MPhysicalLinksState - Simply forward the request to iLinksMgr
TInt CHCIFacade::MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetConnectionHandles(aConnectionHandles, aLinkType);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetNumPendingHandles(aConnectionHandles, aLinkType);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetConnectionHandles(aConnectionHandles, aLinkType, aBDAddr);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetNumPendingHandles(aConnectionHandles, aLinkType, aBDAddr);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetRemoteAddress(TBTDevAddr& aBDAddr, THCIConnHandle aConnectionHandle) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetRemoteAddress(aBDAddr, aConnectionHandle);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetRemoteDeviceClass(TBTDeviceClass& aDeviceClass, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetRemoteDeviceClass(aDeviceClass, aBDAddr);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetRemoteSupportedFeatures(TBTFeatures& aRemoteSupportedFeatures, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetRemoteSupportedFeatures(aRemoteSupportedFeatures, aBDAddr);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetLinkPolicySettings(TLinkPolicy& aLinkPolicySettings, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetLinkPolicySettings(aLinkPolicySettings, aBDAddr);
		}
	return KErrNotReady;
	}

TInt CHCIFacade::MplsGetBasebandLinkState(TBTBasebandLinkState& aBasebandLinkState, const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	if (iLinksMgr)
		{
		return iLinksMgr->GetBasebandLinkState(aBasebandLinkState, aBDAddr);
		}
	return KErrNotReady;
	}

// MLinkMuxNotifier
void CHCIFacade::TryToSend()
	{
	LOG_FUNC
	if (iLinkMuxer)
		{
		iLinkMuxer->TryToSend();
		}
	}

// from MHCIClientUsage
void CHCIFacade::MhcuOpenClientReference()
	{
	iLinkMgrProtocol.LocalOpen();
	}

void CHCIFacade::MhcuCloseClientReference()
	{
	iLinkMgrProtocol.LocalClose();
	}

// Should only be tracking outstanding commands during cmdQ initialisation
TInt CHCIFacade::AddOutstandingCommandOpCode(THCIOpcode aOpCode)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iInitState != EInitialised, Panic(EHCICmdQNotInitialising));
	TUint32 opcode = static_cast<TUint32>(aOpCode);
	return iOutstandingCommands.Append(opcode);	
	}

// Should only be tracking outstanding commands during cmdQ initialisation	
TInt CHCIFacade::FindOutstandingCommandOpCode(THCIOpcode aOpCode) const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iInitState != EInitialised, Panic(EHCICmdQNotInitialising));
	TUint32 opcode = static_cast<TUint32>(aOpCode);
	return iOutstandingCommands.Find(opcode);	
	}
	
// TBasebandPolicy

void TBasebandPolicy::InitialPolicy(const TBasebandPolicyParams& aParams)
	{
	LOG_FUNC
	iPageTimePolicy = aParams.iPageTimePolicy;
	}

TInt TBasebandPolicy::TryToChangePolicy(const TBasebandPolicyParams& aNewParams)
	{
	LOG_FUNC
	// just deal with pagetime for now
	TBasebandPageTimePolicy& current = iPageTimePolicy;

	switch (current)
		{
		case EPagingDontCare:
			break;

		case EPagingNormal:
			current = aNewParams.iPageTimePolicy;
			break;

		case EPagingBestEffort:
			//only change if user wants Quick paging
			current = (aNewParams.iPageTimePolicy == EPagingQuick) ? aNewParams.iPageTimePolicy : current;
			break;

		case EPagingQuick:
			//only change if user wants Best effort paging
			current = (aNewParams.iPageTimePolicy == EPagingBestEffort) ? aNewParams.iPageTimePolicy : current;
			break;

		default:
			LOG(_L("TBasebandPolicy: bogus policy requested for pagetimeout"));
			return KErrArgument;
		}

	LOG1(_L("TBasebandPolicy: now using page time policy %d"), aNewParams.iPageTimePolicy);
	return (current == aNewParams.iPageTimePolicy) ? KErrNone : KErrInUse;
	}


TBasebandPageTimePolicy TBasebandPolicy::PageTimePolicy() const
	{
	LOG_FUNC
	return iPageTimePolicy;
	}

//class CAFHTimer
CAFHTimer* CAFHTimer::NewL(CHCIFacade& aParent)
	{
	LOG_STATIC_FUNC
	CAFHTimer* self = new (ELeave) CAFHTimer(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CAFHTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

CAFHTimer::CAFHTimer(CHCIFacade& aParent)
: CTimer(CActive::EPriorityStandard), iParent(aParent), iPending(EFalse)
	{
	LOG_FUNC
	}
		
void CAFHTimer::SetPending(const TBTAFHHostChannelClassification& aHCC)
	{
	LOG_FUNC
	iHCC.Copy(aHCC);
	iPending=ETrue;
	}
	
void CAFHTimer::RemovePendingHostChannelClassifications()
	{
	LOG_FUNC
	iHCC.Reset();
	iPending=EFalse;
	}
	
void CAFHTimer::Reset()
	{
	LOG_FUNC
	RemovePendingHostChannelClassifications();
	Cancel();
	}
	
void CAFHTimer::RunL()
	{
	LOG_FUNC
	if(iPending)
		{
		if(iStatus==KErrNone)
			{
			iParent.SetAFHHostChannelClassificationL(iHCC);
			}
		RemovePendingHostChannelClassifications();
		}
	}

TInt CAFHTimer::RunError(TInt /*aError*/)
	{
	LOG_FUNC
	return KErrNone;
	}

