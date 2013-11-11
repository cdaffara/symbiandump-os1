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
// This file is made up of 4 different sections:
// - Command Complete processing functions
// These are private functions that implement a specific command 
// complete event, one per handled opcode.
// - Event processing functions
// These are private functions that implement a specific
// command event, one per handled event. These events may or may not
// be matched to a particular command. 
// - Data event processing functions
// These are private functions that implement a specific data event,
// one per handled event.
// - HCI APIs
// These are the functions that are called directly by the 
// different parts of the HCI and make up the MHCICommandQueueClient 
// and MHCIDataEventObserver interfaces.
// 
//

#include <bluetooth/logger.h>
#include "hcifacade.h"
#include "physicallinksmanager.h"
#include "linkmgr.h"
#include "hostresolver.h"
#include "linkutil.h"
#include "linkmuxer.h"
#include "linkflowcontrol.h"
#include "linkconsts.h"
#include "VendorSAP.h"
#include "vendorspecific.h"
#include "AclDataQController.h"
#include "pairingserver.h"
#include "oobdata.h"

#include <e32std.h>
#include <bt_sock.h>
#include <utf.h>
#include <es_ini.h>
#include <btextnotifiers.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hci/hciopcodes.h>

#include <bluetooth/hci/command.h>

// Command Events
#include <bluetooth/hci/connectioncompleteevent.h>
#include <bluetooth/hci/connectionrequestevent.h>
#include <bluetooth/hci/disconnectioncompleteevent.h>
#include <bluetooth/hci/authenticationcompleteevent.h>
#include <bluetooth/hci/encryptionchangeevent.h>
#include <bluetooth/hci/readremsuppfeatcompleteevent.h>
#include <bluetooth/hci/readremoteextendedfeaturescompleteevent.h>
#include <bluetooth/hci/readremverinfocompleteevent.h>
#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/hardwareerrorevent.h>
#include <bluetooth/hci/rolechangeevent.h>
#include <bluetooth/hci/numberofcompletedpacketsevent.h>
#include <bluetooth/hci/modechangeevent.h>
#include <bluetooth/hci/pincoderequestevent.h>
#include <bluetooth/hci/linkkeyrequestevent.h>
#include <bluetooth/hci/linkkeynotificationevent.h>
#include <bluetooth/hci/maxslotschangeevent.h>
#include <bluetooth/hci/readclockoffsetevent.h>
#include <bluetooth/hci/connectionpackettypechangedevent.h>
#include <bluetooth/hci/linksupervisiontimeoutchangedevent.h>
#include <bluetooth/hci/synchronousconnectioncompleteevent.h>
#include <bluetooth/hci/vendordebugevent.h>
#include <bluetooth/hci/vendordebugcompleteevent.h>
#include <bluetooth/hci/writesimplepairingmodecommand.h>
#include <bluetooth/hci/readlocalsupportedcommandscommand.h>
#include <bluetooth/hci/encryptionkeyrefreshcompleteevent.h>


// Command Complete Events
#include <bluetooth/hci/writelinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/readbdaddrcompleteevent.h>
#include <bluetooth/hci/readbuffersizecompleteevent.h>
#include <bluetooth/hci/readclassofdevicecompleteevent.h>
#include <bluetooth/hci/readlocalversioninfocompleteevent.h>
#include <bluetooth/hci/flushcompleteevent.h>
#include <bluetooth/hci/readlocalsupportedfeaturescompleteevent.h>
#include <bluetooth/hci/readlocalsupportedcommandscompleteevent.h>
#include <bluetooth/hci/readlocaloobdatacompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestreplycompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestnegativereplycompleteevent.h>
#include <bluetooth/hci/readinquiryresponsetransmitpowerlevelcompleteevent.h>

// Related commands

#include <bluetooth/hci/readremoteextendedfeaturescommand.h>
#include <bluetooth/hci/readinquiryresponsetransmitpowerlevelcommand.h>
#include <bluetooth/hci/hostbuffersizecommand.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCI_FACADE);
#endif

// ----------------------------------------------------------------------------
// Command Complete processing functions
// ----------------------------------------------------------------------------

void CHCIFacade::WriteLinkPolicySettingsOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	THCIConnHandle connH(KInvalidConnectionHandle);
	
	if (aEvent != NULL)
		{
		const TWriteLinkPolicySettingsCompleteEvent& writeLinkPolicyCompleteEvent = TWriteLinkPolicySettingsCompleteEvent::Cast(*aEvent);
		connH = writeLinkPolicyCompleteEvent.ConnectionHandle();
		}
		
	iLinksMgr->WriteLinkPolicySettingsCompleteEvent(aHciErr, connH);
	}

void CHCIFacade::ResetOpcode(THCIErrorCode /*aHciErr*/, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	// We only expect a reset command event when we are initialising
	__ASSERT_ALWAYS(iInitState == EResetting, Panic(EHCICmdQNotInitialising));
	iInitState = EReset;
	if (iControllerInitialisor != NULL)
		{
		iControllerInitialisor->MciiPostResetCommand();
		}
	else
		{
		// There is no initialisation plugin so behave as if there
		// were and it had completed its post reset phase
		McioPostResetCommandComplete(KErrNone);
		}
	}

void CHCIFacade::ReadBdaddrOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
/**
	Got the local address
	Tell LinkMgr protocol
**/
	{
	LOG_FUNC
	if ((aHciErr == EOK) && (aEvent != NULL))
		{
		const TReadBdaddrCompleteEvent& readBdaddrCompleteEvent = TReadBdaddrCompleteEvent::Cast(*aEvent);
		iLinkMgrProtocol.SetLocalBTAddress(readBdaddrCompleteEvent.BDADDR());
		}
	// do nothing if error
	}

void CHCIFacade::ReadBufferSizeOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	if ((aHciErr == EOK) && (aEvent != NULL))
		{
		const TReadBufferSizeCompleteEvent& readBufSizeCompleteEvent = TReadBufferSizeCompleteEvent::Cast(*aEvent);

		TUint aclPacketLen = readBufSizeCompleteEvent.HCACLDataPacketLength();
		TUint scoPacketLen = readBufSizeCompleteEvent.HCSynchronousDataPacketLength();
		TUint noAclPackets = readBufSizeCompleteEvent.HCTotalNumACLDataPackets();
		TUint noScoPackets = readBufSizeCompleteEvent.HCTotalNumSynchronousDataPackets();

		iLinkMuxer->HandleLocalReadBufferSizeResult(aclPacketLen, scoPacketLen, noAclPackets, noScoPackets);
		LOG3(_L("Link [HCIFacade_Events.cpp]: ReadBufferSizeResult - aErr %d, aAclMaxLen %d, aNoACL %d"), aHciErr, aclPacketLen, noAclPackets);
		}
	}

void CHCIFacade::ReadClassOfDeviceOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	TUint cod(0);
	
	if ((aHciErr == EOK) && (aEvent != NULL))
		{
		const TReadClassOfDeviceCompleteEvent& readCoDCompleteEvent = TReadClassOfDeviceCompleteEvent::Cast(*aEvent);
		cod = readCoDCompleteEvent.ClassOfDevice();
		}

	LOG2(_L("HCIFacade: ReadClassOfDevice Result Event (%d, 0x%x)"), aHciErr, cod);
	iLinkMgrProtocol.UpdateDeviceClass(aHciErr == EOK, cod);
	}

void CHCIFacade::WriteLocalNameOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: SetLocalName Command Complete Event"));
	iLinkMgrProtocol.InquiryMgr().SetLocalNameComplete(CHciUtil::SymbianErrorCode(aHciErr));
	iLinkMgrProtocol.UpdateLocalDeviceName(aHciErr == EOK);
	}

void CHCIFacade::WriteCurrentIACLAPOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: WriteCurrentIACLAP Command Complete Event"));
	
	iLinkMgrProtocol.UpdateLimitedDiscoverable(aHciErr == EOK);
	}

void CHCIFacade::WriteClassOfDeviceOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: WriteClassOfDevice Command Complete Event"));

	iLinkMgrProtocol.UpdateDeviceClass(aHciErr == EOK);
	}

void CHCIFacade::SetControllerToHostFlowControlOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG1(_L8("HCIFacade: SetControllerToHostFlowControl Command Complete Event (result = 0x%02x)"), aHciErr);
	if(aHciErr == EOK)
		{
		// flow control mode set - so provide the host buffer settings
		CHCICommandBase* command = NULL;
		TRAPD(err, command = CHostBufferSizeCommand::NewL(KStackACLBuffersSize, KStackSCOBuffersSize, 
											KStackACLBuffersNum, KStackSCOBuffersNum));
		if(err == KErrNone)
			{
			err = SendInitialisationCommand(command);
			}
		if(err != KErrNone)
			{
			// unfortunately at this stage we are stuck since we need to inform
			// the controller of our buffers.  We've failed to initialise.
			LOG1(_L8("Failed to send HostBufferSize command(%d) - initialisation failed"), err);
			iInitialisationError = ETrue;
			}
		}
	else // we'll drop back to no flow control to the host
		{
		iLinkMuxer->RecordHostControllerToHostFlowControl(EFalse);
		}
	}

void CHCIFacade::HostBufferSizeOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG1(_L8("HCIFacade: HostBufferSizeOpcode Command Complete Event (result = 0x%02x)"), aHciErr);
	if(aHciErr == EOK)
		{
		// all set-up for controller to host flow-control
		iLinkMuxer->RecordHostControllerToHostFlowControl(ETrue);
		}
	else
		{
		// If we've failed to perform this command then the stack is stuck
		// half initialised to perform controller to host flow control.
		LOG(_L8("Failed to set the host buffer size in controller - initialisation failed"));
		iInitialisationError = ETrue;
		}
	}

void CHCIFacade::WriteScanEnableOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: WriteScanEnable Command Complete Event"));

	iLinkMgrProtocol.UpdateLocalDeviceScanEnable(aHciErr == EOK);
	}

void CHCIFacade::SetAFHHostChannelClassificationOpcode(THCIErrorCode /*aHciErr*/, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: WriteAFHHostChannelClassification Command Complete Event"));

	//Update timer to fire after required interval
	//between AFH Host Channel Classification Commands 
	iAFHTimer->Cancel();
	iAFHTimer->After(KAFHHostChannelClassificationIntervalTimer);
	}

void CHCIFacade::WriteAFHChannelAssessmentModeOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("HCIFacade: WriteAFHChannelAssessmentMode Command Complete Event"));
	//If 'status' is not EOK, the registry will not be updated. In particular, if
	//channel assessment is not supported the AFHChannelAssessmentMode
	//will remain set to ETrue. As this value is only used to update the 
	//controller, it does not matter.
	iLinkMgrProtocol.UpdateAFHChannelAssessmentMode(aHciErr == EOK);
	}


void CHCIFacade::ReadLocalVersionInfoOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	TBTDevHCIVersion hciVersion;
	TBTDevLMPVersion lmpVersion;

	if (aEvent != NULL)
		{
		const TReadLocalVersionInfoCompleteEvent& readLocalVersionCompleteEvent = TReadLocalVersionInfoCompleteEvent::Cast(*aEvent);
		
		hciVersion.iHCIVersion = readLocalVersionCompleteEvent.Version();
		hciVersion.iHCIRevision = readLocalVersionCompleteEvent.Revision();
		lmpVersion.iLMPVersion = readLocalVersionCompleteEvent.LMPVersion();
		lmpVersion.iManufacturerID = readLocalVersionCompleteEvent.ManufacturerName();
		lmpVersion.iLMPSubVersion = readLocalVersionCompleteEvent.LMPSubversion();
		}
	iLinkMgrProtocol.SetLocalVersion(aHciErr, hciVersion, lmpVersion);
	
	if(aHciErr == EOK && aEvent)
		{
		if(hciVersion.iHCIVersion > EHWHCIv1_1) // Only send a supported commands command
			{
			CReadLocalSupportedCommandsCommand* cmd = NULL;
			TRAPD(err, cmd = CReadLocalSupportedCommandsCommand::NewL());
			if(err == KErrNone)
				{
				static_cast<void>(SendInitialisationCommand(cmd));
				}
			}
		}

	iReadLocalVersionComplete = ETrue;
	if (iReadLocalSupportedFeaturesComplete)
		{
		SetSupportedEventMasks();		
		}
	}

void CHCIFacade::SetSupportedEventMasks()
	{
	TBTDevHCIVersion hciVersion;
	TUint32 supportedEventMaskLoBytes;
	TUint32 supportedEventMaskHiBytes;
	
	// We Dont mask the LoBytes events yet
	supportedEventMaskLoBytes = THCIEventMask::KDefaultLoBytes;
	supportedEventMaskHiBytes = THCIEventMask::KDefaultHiBytes;
	
	hciVersion = iLinkMgrProtocol.GetHWHCIVersion();
	
	if (hciVersion.iHCIVersion < EHWHCIv1_2)
		{
		// By default we support the core spec v1.2 however the HiBytes for the supported
		// events are not used pre v1.2 and by clearing them it allows us to work with old 
		// controllers if required
		supportedEventMaskHiBytes = 0;
		}		

	if (iLinkMgrProtocol.IsExtendedInquiryResponseSupportedLocally())
		{ 
		supportedEventMaskHiBytes |=	THCIEventMask::EExtendedInquiryResultEvent;
		}
	
	if (iLinkMgrProtocol.IsSecureSimplePairingSupportedLocally())
		{  
		supportedEventMaskHiBytes |=	THCIEventMask::EIOCapabilityRequestEvent|
										THCIEventMask::EIOCapabilityRequestReplyEvent|
										THCIEventMask::EUserConfirmationRequestEvent|
										THCIEventMask::EUserPasskeyRequestEvent|
										THCIEventMask::ERemoteOOBDataRequestEvent|
										THCIEventMask::ESimplePairingCompleteEvent|
										THCIEventMask::EUserPasskeyNotificationEvent|
										THCIEventMask::EKeypressNotificationEvent;
		}
	
	if (hciVersion.iHCIVersion == EHWHCIv2_1)
		{  
		supportedEventMaskHiBytes |=	THCIEventMask::ERemoteHostSupportedFeaturesNotificationEvent;
		supportedEventMaskHiBytes |=	THCIEventMask::ELinkSupervisionTimeoutChangedEvent;
		supportedEventMaskHiBytes |=	THCIEventMask::EEncryptionKeyRefreshCompleteEvent;
		supportedEventMaskHiBytes |=	THCIEventMask::ERemoteHostSupportedFeaturesNotificationEvent;
		}		
	
	THCIEventMask eventMask = {supportedEventMaskLoBytes, supportedEventMaskHiBytes};
	TRAPD(err, SetEventMaskL(eventMask));
	__ASSERT_DEBUG(err == KErrNone, Panic(EHCIControllerInitialisation));
	(void) (err != err);
	}

void CHCIFacade::ReadLocalSupportedFeaturesOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	if ((aHciErr == EOK) && (aEvent != NULL))
		{
		const TReadLocalSupportedFeaturesCompleteEvent& readLocalFeaturesCompleteEvent = TReadLocalSupportedFeaturesCompleteEvent::Cast(*aEvent);
		TBTFeatures features(readLocalFeaturesCompleteEvent.LMPFeatures());
		iLinkMgrProtocol.SetLocalFeatures(aHciErr, features);

		if(iLinkMgrProtocol.IsSecureSimplePairingSupportedLocally())
			{
			CWriteSimplePairingModeCommand* cmd = NULL;
			TRAPD(err, cmd = CWriteSimplePairingModeCommand::NewL(ESimplePairingEnabled));
			if(err == KErrNone)
			    {
                err = SendInitialisationCommand(cmd);
			    }
			if(err != KErrNone)
			    {
                LOG(_L("HCIFacade: ReadLocalSupportedFeaturesOpcode Error"));
                iInitialisationError = ETrue;
			    }
			}
		else
			{
			iLinksMgr->SecMan().SetLocalSimplePairingMode(EFalse);
			}
		}
	else
		{
		iLinkMgrProtocol.SetLocalFeatures(aHciErr, TBTFeatures(0));
		iLinksMgr->SecMan().SetLocalSimplePairingMode(EFalse); // for want of a better solution
		}

	iReadLocalSupportedFeaturesComplete = ETrue;
	if (iReadLocalVersionComplete)
		{
		SetSupportedEventMasks();		
		}

	}

void CHCIFacade::ReadLocalSupportedCommandsOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	if ((aHciErr == EOK) && (aEvent != NULL))
		{
		const TReadLocalSupportedCommandsCompleteEvent& readLocalCommandsCompleteEvent = TReadLocalSupportedCommandsCompleteEvent::Cast(*aEvent);
		TBluetoothHciCommands commands(readLocalCommandsCompleteEvent.SupportedCommands());
		iLinkMgrProtocol.SetLocalCommands(aHciErr, commands);

		if (iLinkMgrProtocol.IsCommandSupportedLocally(ESupportedReadInquiryResponseTransmitPowerCommand))
			{
			CReadInquiryResponseTransmitPowerLevelCommand* cmd = NULL;
			TRAP_IGNORE(cmd = CReadInquiryResponseTransmitPowerLevelCommand::NewL());
			if(cmd)
				{
				static_cast<void>(SendInitialisationCommand(cmd));
				}
			}

		}
	else
		{
		TBluetoothHciCommands commands;
		iLinkMgrProtocol.SetLocalCommands(aHciErr, commands);
		}
	}

void CHCIFacade::FlushOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	THCIConnHandle connH(KInvalidConnectionHandle);

	if (aEvent != NULL)
		{
		const TFlushCompleteEvent& flushCompleteEvent = TFlushCompleteEvent::Cast(*aEvent);
		connH = flushCompleteEvent.ConnectionHandle();
		}

	LOG(_L("Link [HCIFacade_Events.cpp]: FlushCompleteEvent"));
	iLinkMgrProtocol.ACLController().FlushComplete(CHciUtil::SymbianErrorCode(aHciErr), connH);
	}

void CHCIFacade::SwitchRoleOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	if (aHciErr != EOK)
		{
		// Role change failed, probably "Role change not allowed", notify clients
		iLinksMgr->RoleChangeRejectedByHW(aHciErr);
		return;						
		}		
	}

void CHCIFacade::SetEventMaskOpcode(THCIErrorCode __DEBUG_ONLY(aHciErr), const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	#ifdef _DEBUG
	LOG1(_L("HCIFacade: SetEventMask Result Event (%d)"), aHciErr); // leave in at all?
	#endif
	__ASSERT_DEBUG(aHciErr ==  EOK, Panic(EHCIControllerInitialisation));
	iLinkMgrProtocol.LocalSupportedFeaturesAvailable();
	}

void CHCIFacade::ReadInquiryResponseTransmitPowerLevelOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	if (aHciErr == EOK && aEvent)
		{
		const TReadInquiryResponseTransmitPowerLevelCompleteEvent& readInquiryResponseTransmitPowerLevelCompleteEvent = TReadInquiryResponseTransmitPowerLevelCompleteEvent::Cast(*aEvent);
		TInt8 txPowerLevel = readInquiryResponseTransmitPowerLevelCompleteEvent.TxPowerLevel();
		iLinkMgrProtocol.UpdateInquiryResponseTxPowerLevel(txPowerLevel);
		FTRACE(FPrint(_L("HCIFacade: TxPowerLevel (%d)"), txPowerLevel));
		}
	}


// ----------------------------------------------------------------------------
// Event processing functions
// ----------------------------------------------------------------------------

void CHCIFacade::ConnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand)
	/**
	   Some connection request has completed.

	   This may be a successful connection complete, or a failure.
	**/
	{
	LOG_FUNC
	TBTConnect conn;
	const TConnectionCompleteEvent& connCompleteEvent = TConnectionCompleteEvent::Cast(aEvent);
	THCIErrorCode err = aEvent.ErrorCode();
	
	conn.iConnH = connCompleteEvent.ConnectionHandle();
	conn.iBdaddr = connCompleteEvent.BDADDR();
	conn.iLinkType = TLinkType(connCompleteEvent.LinkType());
	conn.iEncryptMode = TEncryptMode(connCompleteEvent.EncryptionMode());
	
	LOG2(_L("Link [HCIFacade_Events.cpp]: Connection complete event. Code %d, link type=%d [from]"), err,conn.iLinkType);
	LOGHEXDESC(conn.iBdaddr.Des());
	iLinksMgr->ConnectionComplete(err, conn);

	if(!aRelatedCommand  ||
		(aRelatedCommand && (aRelatedCommand->Opcode() == KCreateACLConnectionOpcode)))
		// this has only to do with paging, that is actively creating connections 
		// to maintain symmetry with updating UI we do it here, and not trouble PHYmgr
		{
		iLinkMgrProtocol.SetUIConnecting(EFalse);
		}
	}

void CHCIFacade::AuthenticationCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TAuthenticationCompleteEvent& authCompleteEvent = TAuthenticationCompleteEvent::Cast(aEvent);
	iLinksMgr->AuthenticationComplete(aEvent.ErrorCode(), authCompleteEvent.ConnectionHandle());
	}

void CHCIFacade::ReadRemSuppFeatCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TReadRemSuppFeatCompleteEvent& readRemSuppFeatCompleteEvent = TReadRemSuppFeatCompleteEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]: ReadRemoteSupportedFeaturesCompleteEvent"));
	
	TBTFeatures features(readRemSuppFeatCompleteEvent.LMPFeatures());
	iLinksMgr->ReadRemoteSupportedFeaturesComplete(aEvent.ErrorCode(), readRemSuppFeatCompleteEvent.ConnectionHandle(), features);
	}

void CHCIFacade::ReadRemoteExtendedFeaturesCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TReadRemoteExtendedFeaturesCompleteEvent& readRemoteExtendedFeaturesCompleteEvent = TReadRemoteExtendedFeaturesCompleteEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]: ReadRemoteExtendedFeaturesCompleteEvent"));
	
	TUint64 features = readRemoteExtendedFeaturesCompleteEvent.ExtendedLMPFeatures();
	TUint8 pageNumber = readRemoteExtendedFeaturesCompleteEvent.PageNumber();
	TUint8 maxPageNumber = readRemoteExtendedFeaturesCompleteEvent.MaximumPageNumber();
	iLinksMgr->ReadRemoteExtendedFeaturesComplete(aEvent.ErrorCode(), readRemoteExtendedFeaturesCompleteEvent.ConnectionHandle(), features, pageNumber, maxPageNumber);
	}


void CHCIFacade::ReadRemVerInfoCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TReadRemVerInfoCompleteEvent& readRemVerInfoCompleteEvent = TReadRemVerInfoCompleteEvent::Cast(aEvent);
	TBTDevRemoteHwVersion ver;
	
	ver.iLMPVersion = readRemVerInfoCompleteEvent.LMPVersion();
	ver.iManufacturerID = readRemVerInfoCompleteEvent.ManufacturerName();
	ver.iLMPSubVersion = readRemVerInfoCompleteEvent.LMPSubversion();

	LOG(_L("Link [HCIFacade_Events.cpp]: ReadRemoteVersionInformationCompleteEvent"));

	iLinksMgr->ReadRemoteVersionInfoComplete(aEvent.ErrorCode(), readRemVerInfoCompleteEvent.ConnectionHandle(), ver);
	}

void CHCIFacade::CommandCompleteEvent(THCIOpcode aOpcode, THCIErrorCode aHciErr, 
										const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	LOG3(_L("aOpcode=%d, aHciErr=%d, iOutstandingCommands.Count()=%d"), aOpcode, aHciErr, iOutstandingCommands.Count());

	switch(aOpcode)
		{
	case KWriteLinkPolicySettingsOpcode:
		WriteLinkPolicySettingsOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KResetOpcode:
		__ASSERT_ALWAYS(iOutstandingCommands.Count()==1, Panic(EHCICtrlrInitOnlyOneResetCmdAllowed));
		ResetOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KWriteLocalNameOpcode:
		WriteLocalNameOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KReadClassOfDeviceOpcode:
		ReadClassOfDeviceOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KWriteCurrentIACLAPOpcode:
		WriteCurrentIACLAPOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KWriteClassOfDeviceOpcode:
		WriteClassOfDeviceOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KSetControllerToHostFlowControlOpcode:
		SetControllerToHostFlowControlOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KHostBufferSizeOpcode:
		HostBufferSizeOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KWriteScanEnableOpcode:
		WriteScanEnableOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KSetAFHHostChannelClassificationOpcode:
		SetAFHHostChannelClassificationOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KWriteAFHChannelAssessmentModeOpcode:
		WriteAFHChannelAssessmentModeOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KReadLocalVersionInfoOpcode:
		ReadLocalVersionInfoOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KReadLocalSupportedFeaturesOpcode:
		ReadLocalSupportedFeaturesOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
	
	case KReadLocalSupportedCommandsOpcode:
		ReadLocalSupportedCommandsOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KReadBufferSizeOpcode:
		ReadBufferSizeOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KReadBdaddrOpcode:
		ReadBdaddrOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KFlushOpcode:
		FlushOpcode(aHciErr, aEvent, aRelatedCommand);
		break;
		
	case KSwitchRoleOpcode:
		SwitchRoleOpcode(aHciErr, aEvent, aRelatedCommand);
		break;

	case KSetEventMaskOpcode:
		SetEventMaskOpcode(aHciErr, aEvent, aRelatedCommand);
		break;

	case KReadInquiryResponseTransmitPowerLevelOpcode:
		ReadInquiryResponseTransmitPowerLevelOpcode(aHciErr, aEvent, aRelatedCommand);
		break;

	// Inquiry related events
	case KInquiryCancelOpcode:
	case KRemoteNameRequestCancelOpcode:
		// Only the inquiry manager issues remote name requests.
		FTRACE(FPrint(_L("Error!! Unsolicited inquiry-type command complete event (opcode: 0x%04x)"), aOpcode));
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnmatchedInquiryEvent));
		break;

    case KWriteSimplePairingModeOpcode:
        {
        WriteSimplePairingModeOpcode(aHciErr, aEvent, aRelatedCommand);
        break;
        }
        
	// Security related events that are sent from the facade.
	case KReadLocalOOBDataOpcode:
	case KRemoteOOBDataRequestReplyOpcode:
	case KRemoteOOBDataRequestNegativeReplyOpcode:
	case KIOCapabilityRequestReplyOpcode:
	case KUserConfirmationRequestReplyOpcode:
	case KUserConfirmationRequestNegativeReplyOpcode:
	case KUserPasskeyRequestReplyOpcode:
	case KUserPasskeyRequestNegativeReplyOpcode:
	case KIOCapabilityRequestNegativeReplyOpcode:
		iLinksMgr->SecMan().HCIEventHandler().MhcqcCommandEventReceived(*aEvent, aRelatedCommand);
		break;
		
	// List of valid command complete event opcodes that we ignore
	case KRejectConnectionRequestOpcode:
	case KReadStoredLinkKeyOpcode:
	case KWriteStoredLinkKeyOpcode:
	case KDeleteStoredLinkKeyOpcode:
	case KReadScanEnableOpcode:
	case KReadEncryptionModeOpcode:
	case KReadNumberOfSupportedIACOpcode:
	case KReadVoiceSettingOpcode:
	case KReadAuthenticationEnableOpcode:
	case KReadCountryCodeOpcode:
	case KReadCurrentIACLAPOpcode:
	case KWriteLinkSupervisionTimeoutOpcode:
	case KLinkKeyRequestReplyOpcode:
	case KLinkKeyRequestReplyNegativeOpcode:
	case KPINCodeRequestReplyOpcode:
	case KPINCodeRequestReplyNegativeOpcode:
	case KRoleDiscoveryOpcode:
	case KPeriodicInquiryModeOpcode:
	case KExitPeriodicInquiryModeOpcode:
	case KSetEventFilterOpcode:
	case KCreateNewUnitKeyOpcode:
	case KWriteAuthenticationEnableOpcode:
	case KWriteEncryptionModeOpcode:
	case KWritePageTimeoutOpcode:
	case KReadConnectionAcceptTimeoutOpcode:
	case KWriteConnectionAcceptTimeoutOpcode:
	case KWriteVoiceSettingOpcode:
	case KReadAFHChannelMapOpcode:
	case KReadAFHChannelAssessmentModeOpcode:
	case KReadPageTimeoutOpcode:
	case KCreateConnectionCancelOpcode:
	case KReadLMPHandleOpcode:
	case KReadLinkPolicySettingsOpcode:
	case KReadDefaultLinkPolicySettingsOpcode:
	case KWriteDefaultLinkPolicySettingsOpcode:	
	case KReadPINTypeOpcode:
	case KWritePINTypeOpcode:		
	case KReadPageScanActivityOpcode:
	case KWritePageScanActivityOpcode:
	case KReadInquiryScanActivityOpcode:
	case KWriteInquiryScanActivityOpcode:
	case KReadAutomaticFlushTimeoutOpcode:
	case KWriteAutomaticFlushTimeoutOpcode:
	case KReadNumBroadcastRetransmissionsOpcode:
	case KWriteNumBroadcastRetransmissionsOpcode:
	case KReadHoldModeActivityOpcode:
	case KWriteHoldModeActivityOpcode:
	case KReadTransmitPowerLevelOpcode:
	case KReadSynchronousFlowControlEnableOpcode:
	case KWriteSynchronousFlowControlEnableOpcode:		
	case KReadLinkSupervisionTimeoutOpcode:
	case KReadPageScanPeriodOpcode:
	case KWritePageScanPeriodOpcode:
	case KReadPageScanOpcode:
	case KWritePageScanOpcode:
	case KReadInquiryScanTypeOpcode:
	case KWriteInquiryScanTypeOpcode:
	case KReadInquiryModeOpcode:
	case KReadPageScanTypeOpcode:
	case KWritePageScanTypeOpcode:
	case KReadLocalExtendedFeaturesOpcode:
	case KReadFailedContactCounterOpcode:
	case KResetFailedContactCounterOpcode:
	case KReadLinkQualityOpcode:
	case KReadRSSIOpcode:
	case KReadClockOpcode:
	case KReadLoopbackModeOpcode:
	case KWriteLoopbackModeOpcode:
	case KEnableDeviceUnderTestModeOpcode:
	case KReadExtendedInquiryResponseOpcode:

	case KReadSimplePairingModeOpcode:
	case KSendKeypressNotificationOpcode:

	case KWriteSimplePairingDebugModeOpcode:
	
	// below here are command complete events we definitely shouldn't receive
	// but have been left for safety until a complete analysis is done.
	case KHostNumberOfCompletedPacketsOpcode:
		
		// Catch all the events we do not handle
		LOG1(_L("Warning!! Unhandled Command Complete Event (opcode:%d)"), aOpcode);
		break;
		
	default:
		THCIOpcode OGF=static_cast<THCIOpcode>(aOpcode&KOGFMask);

		if(OGF==KVendorDebugOGF)
			{
			LOG(_L("Link: [HCIFacade_Events.cpp]: Vendor Specfic Command Complete Event"));
			if(iLinkMgrProtocol.VendorSpecificSAP())
				{
				if (aEvent)
					{
					TPtrC8 eventPacket(TVendorDebugCompleteEvent::Cast(*aEvent).VendorDebugData());
					iLinkMgrProtocol.VendorSpecificSAP()->VendorCommandCompleteEvent(aHciErr, &eventPacket);
					}
				else
					{
					FLOG(_L("Link: [HCIFacade_Events.cpp]: Vendor Specific Command Complete Event is NULL"));
					iLinkMgrProtocol.VendorSpecificSAP()->VendorCommandCompleteEvent(aHciErr, NULL);
					}
				}
			break;
			}

		LOG2(_L("Link [HCIFacade_Events.cpp]: Warning: Unknown Command complete event! Opcode %d error code %d"), aOpcode, aHciErr);

		__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownCommandCompleteOpcode));			
		break;				
		}
		
	if(iInitState == EPostReset)
		{
		__ASSERT_ALWAYS(iOutstandingCommands.Count() > 0, Panic(EHCICtrlrInitNoOutstandingCmds));
		TInt position(FindOutstandingCommandOpCode(aOpcode));
		
		if (position < 0)
			{
			// HCI matches the incoming event to a command which stack has not sent and hence we must panic.
			// More explicitly HCI cmdQ has got a command (aRelatedCommand != NULL) which was not sent by stack. 
			// (position <0 i.e. not found)
			
			__ASSERT_ALWAYS(!aRelatedCommand, Panic(EHCICtrlrInitCmdNotFound));
			}
		else
			{
			iOutstandingCommands.Remove(position);
			if (!iInitialisationError && iOutstandingCommands.Count() == 0)
				{
				// start the CmdQ
				iCmdController->Start();
				iInitState = EInitialised;
				// if we haven't error'd, power is ON
				iLinkMgrProtocol.UpdateLocalDevicePower(EBTOn);	
				}
			}
		}
	else if(iInitState == EReset)
		{
		// If an initialisor is present, ResetOpcode() calls its MciiPostResetCommand()
		// function. If this sends an async command, it does not call McioPostResetCommandComplete(),
		// and iInitState will not get updated and still be set to EReset.
		// We should remove the command from the queue, without updating iInitState.
		__ASSERT_ALWAYS(iOutstandingCommands.Count() == 1, Panic(EHCICtrlrInitNoOutstandingCmds));
		__ASSERT_DEBUG(aOpcode == KResetOpcode, Panic(EHCICtrlrInitCmdNotFound));
		TInt position(FindOutstandingCommandOpCode(aOpcode));
		__ASSERT_ALWAYS(position >= 0, Panic(EHCICtrlrInitCmdNotFound));
		iOutstandingCommands.Remove(position);		
		}
	else if(iInitState == EResetting)
		{
		__ASSERT_ALWAYS(iOutstandingCommands.Count()==1, Panic(EHCICtrlrInitOnlyOneResetCmdAllowed));
		}
	else
		{
		__ASSERT_ALWAYS(iOutstandingCommands.Count()==0, Panic(EHCICtrlrInitFailedToRemoveCmd));
		}
	}

void CHCIFacade::WriteSimplePairingModeOpcode(THCIErrorCode aHciErr, const THCIEventBase* /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
    {
    LOG_FUNC
    if(aHciErr == EOK)
        {
        iLinksMgr->SecMan().SetLocalSimplePairingMode(ETrue);
        }
    else
        {
        iInitialisationError = ETrue;
        }
    }

void CHCIFacade::CommandStatusEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	const TCommandStatusEvent& commandStatusEvent = TCommandStatusEvent::Cast(aEvent);
	THCIOpcode opcode = commandStatusEvent.CommandOpcode();
	THCIErrorCode hciErr = commandStatusEvent.ErrorCode();
	
	if (hciErr != EOK)
		{
		// got an error
		// map onto the event that would have occurred: some things we will have to let the client work out
		// e.g. they should check error and connection handle etc.
		switch (opcode)
			{
		case KCreateACLConnectionOpcode:
		case KAcceptConnectionRequestOpcode:
			{
			TBTConnect nulldevice;
			nulldevice.iBdaddr = MAKE_TINT64(0,0);
			nulldevice.iLinkType = EACLLink;
			iLinksMgr->ConnectionComplete(hciErr, nulldevice);

			if(opcode == KCreateACLConnectionOpcode)
				// this has only to do with paging, that is actively creating connections 
				// to maintain symmetry with updating UI we do it here, and not trouble PHYmgr
				{
				iLinkMgrProtocol.SetUIConnecting(EFalse);
				}
			break;
			}
			
		case KAuthenticationRequestedOpcode:
			iLinksMgr->AuthenticationComplete(hciErr, KInvalidConnectionHandle);
			break;
			
		case KDisconnectOpcode:
			iLinksMgr->Disconnection(hciErr, KInvalidConnectionHandle, hciErr); // reason=aStatus
			break;
			
		case KAddSCOConnectionOpcode:
			{
			TBTConnect nulldevice;
			nulldevice.iBdaddr = MAKE_TINT64(0,0);
			nulldevice.iLinkType = ESCOLink;
			iLinksMgr->ConnectionComplete(hciErr, nulldevice);
			break;
			}
			
		case KSetConnectionEncryptionOpcode:
			iLinksMgr->EncryptionChange(hciErr, KInvalidConnectionHandle, EFalse); // no encryption
			break;
			
		case KReadRemoteSupportedFeaturesOpcode:
			iLinksMgr->ReadRemoteSupportedFeaturesComplete(hciErr, KInvalidConnectionHandle, TBTFeatures(0)); // no bitmask
			break;
			
		case KReadClockOffsetOpcode:
			iLinksMgr->ClockOffset(hciErr, KInvalidConnectionHandle, 0);
			break;
			
		case KReadRemoteVersionInfoOpcode:
			{
			TBTDevRemoteHwVersion nullversion;
			nullversion.iLMPVersion=0;
			nullversion.iManufacturerID=0;
			nullversion.iLMPSubVersion=0;
		
			iLinksMgr->ReadRemoteVersionInfoComplete(hciErr, KInvalidConnectionHandle, nullversion);
			break;
			}
			
		case KSetupSynchronousConnectionOpcode:
		case KAcceptSynchronousConnectionRequestOpcode:
			{
		    TBTConnect conn;
		    conn.iConnH = 0;
		    conn.iLinkType = EeSCOLink;

			TBTSyncConnectOpts syncOpts(0, 0, 0, 0, EuLawLog);

			iLinksMgr->SynchronousConnectionComplete(hciErr, conn, syncOpts);
			break;
			}
		
		case KReadRemoteExtendedFeaturesOpcode:
			{
			// Maybe migrate this to use HCIv2 so command is owned by physical link
			__ASSERT_ALWAYS(aRelatedCommand, Panic(EHCIUnexpectedEvent));
			__ASSERT_ALWAYS(aRelatedCommand->Opcode() == KReadRemoteExtendedFeaturesOpcode, Panic(EHCICommandBadArgument));
			const CReadRemoteExtendedFeaturesCommand* cmd = reinterpret_cast<const CReadRemoteExtendedFeaturesCommand*>(aRelatedCommand);
			iLinksMgr->ReadRemoteExtendedFeaturesComplete(hciErr, cmd->ConnectionHandle(), TUint64(0), cmd->PageNumber(), 0);
			break;
			}
		// Deal with low power mode request failures. 
		// We only change state when we receive successful responses to low power 
		// mode requests. Errors can be safely ignored as we haven't changed state
		// at this time. 
		// Whilst it would be preferable to handle these responses doing so would 
		// require substantial changes to the functions that handle low power mode 
		// requests in order for them to take full advantage of HCIv2. At present 
		// this problem only occurs in debug mode and so this is solution is preferable. 
		case KHoldModeOpcode:
		case KParkModeOpcode:
		case KExitParkModeOpcode:
		case KSniffModeOpcode:
		case KExitSniffModeOpcode:
			{
			break;
			}
		case KChangeConnectionPacketTypeOpcode:	
			{
			iLinksMgr->PacketTypeChange(hciErr, KInvalidConnectionHandle,0);
			break;
			}
        
        case KWriteSimplePairingModeOpcode:
            {
            WriteSimplePairingModeOpcode(hciErr, &commandStatusEvent, aRelatedCommand);
            break;
            }
	
		default:
			// Complete any other commands with an error
			CommandCompleteEvent(opcode, hciErr, NULL, NULL);
			break;	
			}
		}
	else // Error free status event
		{
		switch (opcode)
			{
		case KCreateACLConnectionOpcode:
			// protocol looks after the ui gubbins
			iLinkMgrProtocol.SetUIConnecting(ETrue);		
			break;
			
		case KAuthenticationRequestedOpcode:
			// tell secman that it really going through authentication
			iLinksMgr->SecMan().AuthenticationInProgress();
			break;
		
		default:
			break;
			}
		}
	}

void CHCIFacade::ReadClockOffsetEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TReadClockOffsetEvent& readClockOffsetEvent = TReadClockOffsetEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]: ReadClockOffsetResultEvent"));
	iLinksMgr->ClockOffset(aEvent.ErrorCode(), readClockOffsetEvent.ConnectionHandle(), readClockOffsetEvent.ClockOffset());
	}

void CHCIFacade::ConnectionPacketTypeChangedEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TConnectionPacketTypeChangedEvent& connPacketTypeChangedEvent = TConnectionPacketTypeChangedEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]:ConnectionPacketTypeChangedEvent"));
	// we need to pass onto the connections manager - this will let the connections know
	// and subsequently the baseband model
	iLinksMgr->PacketTypeChange(aEvent.ErrorCode(), connPacketTypeChangedEvent.ConnectionHandle(), connPacketTypeChangedEvent.PacketType());
	}

void CHCIFacade::LinkSupervisionTimeoutChangedEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TLinkSupervisionTimeoutChangedEvent& linkSupervisionTimeoutChangedEvent = TLinkSupervisionTimeoutChangedEvent::Cast(aEvent);

	iLinksMgr->LinkSupervisionTimeoutChange(aEvent.ErrorCode(),
											linkSupervisionTimeoutChangedEvent.ConnectionHandle(),
											linkSupervisionTimeoutChangedEvent.LinkSupervisionTimeout());
	}

void CHCIFacade::SynchronousConnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TSynchronousConnectionCompleteEvent& syncConnCompleteEvent = TSynchronousConnectionCompleteEvent::Cast(aEvent);
	
    TBTConnect conn;
    conn.iConnH = syncConnCompleteEvent.ConnectionHandle();
    conn.iBdaddr = syncConnCompleteEvent.BDADDR();
    conn.iLinkType = TLinkType(syncConnCompleteEvent.LinkType());
	
	LOG(_L("Link [HCIFacade_Events.cpp]: Synchronous connection complete [from]"));
	LOGHEXDESC(conn.iBdaddr.Des());

	TBTSyncConnectOpts syncOpts(syncConnCompleteEvent.TransmissionInterval(), 
								syncConnCompleteEvent.RetransmissionWindow(),
								syncConnCompleteEvent.RxPacket_Length(),
								syncConnCompleteEvent.TxPacket_Length(),
								TAirMode(syncConnCompleteEvent.AirMode()));

	iLinksMgr->SynchronousConnectionComplete(aEvent.ErrorCode(), conn, syncOpts);
	}

void CHCIFacade::VendorDebugEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	LOG(_L("Link [HCIFacade_Events.cpp]: **WARNING** - Just received a Vendor Specific Debug Event !"));

	if(iLinkMgrProtocol.VendorSpecificSAP())
		{
		const TVendorDebugEvent& vendorEvent = TVendorDebugEvent::Cast(aEvent);

		const TDesC8& des = vendorEvent.VendorDebugData();
		iLinkMgrProtocol.VendorSpecificSAP()->VendorDebugEvent(aEvent.ErrorCode(), &des);
		}	
	}

void CHCIFacade::ConnectionRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
/**
   A connection request has come in.
   
   Tell connections mgr = it will decide whether to accept or not
**/
	{
	LOG_FUNC
	TBTConnect conn;
	const TConnectionRequestEvent& connRequestEvent = TConnectionRequestEvent::Cast(aEvent);
	THCIErrorCode err = aEvent.ErrorCode();
	
	conn.iBdaddr = connRequestEvent.BDADDR();
	conn.iCoD = connRequestEvent.ClassOfDevice();
	conn.iLinkType = TLinkType(connRequestEvent.LinkType());

	LOG(_L("Link [HCIFacade_Events.cpp]: Connection request [from]"));
	LOGHEXDESC(conn.iBdaddr.Des());
	
	iLinksMgr->ConnectionRequest(conn);	// passes Address and CoD
	}

void CHCIFacade::DisconnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
/**
   Disconnect indication up from hardware.
   
   Called for local or remote initiated baseband disconnect, or on
   hardware failure.
   Any flushed ACL data will be indicated up to us seperately.
   
   @param aErr    HCI Error code -- if not EOK we didn't actually connect.
   @param aConnH  Handle the disconnect happended on.
   @param aReason If aErr == EOK, this tells us why we disconnect. Else not valid.
**/
	{
	LOG_FUNC
	const TDisconnectionCompleteEvent& disconnCompleteEvent = TDisconnectionCompleteEvent::Cast(aEvent);
	THCIErrorCode err = aEvent.ErrorCode();
	THCIConnHandle connH = disconnCompleteEvent.ConnectionHandle();
	THCIErrorCode reason = THCIErrorCode(disconnCompleteEvent.Reason());
	
	LOG2(_L("HCI: disconnection complete event, handle %d, error %d"), connH, err);
	iLinksMgr->Disconnection(err, connH, reason);
	}

void CHCIFacade::EncryptionChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TEncryptionChangeEvent& encryptChangeEvent = TEncryptionChangeEvent::Cast(aEvent);
	iLinksMgr->EncryptionChange(aEvent.ErrorCode(), encryptChangeEvent.ConnectionHandle(), encryptChangeEvent.EncryptionEnable());
	}

void CHCIFacade::HardwareErrorEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const THardwareErrorEvent& hardwareErrorEvent = THardwareErrorEvent::Cast(aEvent);
	TUint8 hwErr = hardwareErrorEvent.HardwareCode();
	
	// Just log event, QDP is responsible for 'rebooting' the stack if required
	LOG1(_L("Link [HCIFacade_Events.cpp]: Error! HardwareErrorEvent Received: %d"), hwErr);
	}

void CHCIFacade::RoleChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TRoleChangeEvent& roleChangeEvent = TRoleChangeEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]: RoleChangeEvent"));
	iLinksMgr->RoleChange(aEvent.ErrorCode(), roleChangeEvent.BDADDR(), TBTBasebandRole(roleChangeEvent.Newrole()));
	}

void CHCIFacade::ModeChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TModeChangeEvent& modeChangeEvent = TModeChangeEvent::Cast(aEvent);
	TBTLinkMode mode = EActiveMode;
	switch(modeChangeEvent.CurrentMode())
		{
	// Mode 0, as defined for the Mode Change Event, is Active Mode
	case 0: 
		break;
		
	case 1: 
		mode = EHoldMode; 
		break;
		
	case 2:	
		mode = ESniffMode;
		break;
		
	case 3:
		mode = EParkMode;
		break;
		
	default:
		__ASSERT_ALWAYS(EFalse, Panic(EHCICommandBadArgument));
		break;
		}

	LOG(_L("Link [HCIFacade_Events.cpp]: ModeChangeEvent"));
	iLinksMgr->ModeChange(aEvent.ErrorCode(), modeChangeEvent.ConnectionHandle(), mode, modeChangeEvent.Interval());
	}

void CHCIFacade::PINCodeRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TPINCodeRequestEvent& pinCodeRequestEvent = TPINCodeRequestEvent::Cast(aEvent);

	iLinksMgr->PinRequest(pinCodeRequestEvent.BDADDR(), *this);
	}

void CHCIFacade::LinkKeyRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TLinkKeyRequestEvent& linkKeyRequestEvent = TLinkKeyRequestEvent::Cast(aEvent);

	// Tell the Connections Manager
	// this is something the Connections look after - they can tell the Registry when they want of the new key
	iLinksMgr->LinkKeyRequest(linkKeyRequestEvent.BDADDR(), *this);
	}

void CHCIFacade::LinkKeyNotificationEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TLinkKeyNotificationEvent& linkKeyNotEvent = TLinkKeyNotificationEvent::Cast(aEvent);

	// Tell the Connections Manager
	// this is something the Connections look after - they can tell the Registry when they want of the new key
	iLinksMgr->NewLinkKey(linkKeyNotEvent.BDADDR(), linkKeyNotEvent.LinkKey(), static_cast<THCILinkKeyType>(linkKeyNotEvent.KeyType()));
	}

void CHCIFacade::MaxSlotsChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TMaxSlotsChangeEvent& maxSlotsChangeEvent = TMaxSlotsChangeEvent::Cast(aEvent);

	LOG(_L("Link [HCIFacade_Events.cpp]: MaxSlotsChangeEvent"));
	iLinksMgr->MaxSlotsChange(maxSlotsChangeEvent.ConnectionHandle(), maxSlotsChangeEvent.LMPMaxSlots());
	}

void CHCIFacade::EncryptionKeyRefreshCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	const TEncryptionKeyRefreshCompleteEvent& refreshCompleteEvent = TEncryptionKeyRefreshCompleteEvent::Cast(aEvent);

	iLinksMgr->EncryptionKeyRefreshComplete(aEvent.ErrorCode(), refreshCompleteEvent.ConnectionHandle());
	}

// ----------------------------------------------------------------------------
// Data event processing functions
// ----------------------------------------------------------------------------

void CHCIFacade::NumberOfCompletedPacketsEvent(THCIEventBase& aEvent)
/**
	Packets sent by HC to remote device.
	For informational scheduling purposes, this connection handle just
	cleared its oldest aNum packets.
**/
	{
	LOG_FUNC
	const TNumberOfCompletedPacketsEvent& numCompletedPacketsEvent = TNumberOfCompletedPacketsEvent::Cast(aEvent);
	TUint numPackets;

	TUint numHandles = numCompletedPacketsEvent.NumberofHandles();
	for (TUint index = 0; index < numHandles; index++)
		{
		numPackets = numCompletedPacketsEvent.HCNumOfCompletedPackets(index);
		
		LOG1(_L("HCIFacade: Notifying DataQ Controller of %d Completed packets"), numPackets);

		iLinksMgr->CompletedPackets(numCompletedPacketsEvent.ConnectionHandle(index), numPackets);
		}
	}

// ----------------------------------------------------------------------------
// HCI APIs
// ----------------------------------------------------------------------------

// MHCICommandQueueClient
void CHCIFacade::MhcqcCommandEventReceived(const THCIEventBase& aEvent,
										   const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	switch(aEvent.EventCode())
		{
	case EConnectionCompleteEvent:
		ConnectionCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case EAuthenticationCompleteEvent:
		AuthenticationCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case EReadRemSuppFeatCompleteEvent:
		ReadRemSuppFeatCompleteEvent(aEvent, aRelatedCommand);
		break;
	
	case EReadRemoteExtendedFeaturesCompleteEvent:
		ReadRemoteExtendedFeaturesCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case EReadRemVerInfoCompleteEvent:
		ReadRemVerInfoCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case ECommandCompleteEvent:
		{
		const THCICommandCompleteEvent* completeEvent = reinterpret_cast<const THCICommandCompleteEvent*>(&aEvent);
		CommandCompleteEvent(completeEvent->CommandOpcode(), aEvent.ErrorCode(), &aEvent, aRelatedCommand);
		break;
		}
		
	case ECommandStatusEvent:
		CommandStatusEvent(aEvent, aRelatedCommand);
		break;
		
	case EReadClockOffsetEvent:
		ReadClockOffsetEvent(aEvent, aRelatedCommand);
		break;
		
	case EConnectionPacketTypeChangedEvent:
		ConnectionPacketTypeChangedEvent(aEvent, aRelatedCommand);
		break;
		
	case ELinkSupervisionTimeoutChangedEvent:
		LinkSupervisionTimeoutChangedEvent(aEvent, aRelatedCommand);
		break;
		
	case ESynchronousConnectionCompleteEvent:
		SynchronousConnectionCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case EConnectionRequestEvent:
		ConnectionRequestEvent(aEvent, aRelatedCommand);
		break;
		
	case EDisconnectionCompleteEvent:
		DisconnectionCompleteEvent(aEvent, aRelatedCommand);
		break;
		
	case EEncryptionChangeEvent:
		EncryptionChangeEvent(aEvent, aRelatedCommand);
		break;
		
	case EHardwareErrorEvent:
		HardwareErrorEvent(aEvent, aRelatedCommand);
		break;
		
	case ERoleChangeEvent:
		RoleChangeEvent(aEvent, aRelatedCommand);
		break;
		
	case EModeChangeEvent:
		ModeChangeEvent(aEvent, aRelatedCommand);
		break;
		
	case EPINCodeRequestEvent:
		PINCodeRequestEvent(aEvent, aRelatedCommand);
		break;
		
	case ELinkKeyRequestEvent:
		LinkKeyRequestEvent(aEvent, aRelatedCommand);
		break;
		
	case ELinkKeyNotificationEvent:
		LinkKeyNotificationEvent(aEvent, aRelatedCommand);
		break;
		
	case EMaxSlotsChangeEvent:
		MaxSlotsChangeEvent(aEvent, aRelatedCommand);
		break;

	case EVendorDebugEvent:
		VendorDebugEvent(aEvent, aRelatedCommand);
		break;
		
	case EEncryptionKeyRefreshCompleteEvent:		
		EncryptionKeyRefreshCompleteEvent(aEvent, aRelatedCommand);
		break;

	// Inquiry based events.
	// By default these should be caused by the inquiry manager and so be returned to it.
	case EInquiryCompleteEvent:
	case EInquiryResultEvent:
	case EInquiryResultwithRSSIEvent:
	case EExtendedInquiryResultEvent:
	case ERemoteNameReqCompleteEvent:
	case ERemoteHostSupportedFeaturesNotificationEvent:
		FTRACE(FPrint(_L("Error!! Unsolicited inquiry-type event (event code:%d)"), aEvent.EventCode()));
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnmatchedInquiryEvent));
		break;

	// Security based events.
	case EIOCapabilityRequestEvent:
	case EIOCapabilityResponseEvent:
	case EUserConfirmationRequestEvent:
	case ERemoteOOBDataRequestEvent:
	case ESimplePairingCompleteEvent:
	case EUserPasskeyNotificationEvent:
	case EKeypressNotificationEvent:
		iLinksMgr->SecMan().HCIEventHandler().MhcqcCommandEventReceived(aEvent, aRelatedCommand);
		break;

	// Catch all the events we do not handle
	case EChangeLinkKeyEvent:
	case EMasterLinkKeyEvent:
	case EQOSSetupCompleteEvent:
	case EReturnLinkKeysEvent:
	case EFlowSpecificationCompleteEvent:
	case ESynchronousConnectionChangedEvent:
	case ELoopbackCommandEvent:
	case EPageScanModeChangeEvent:
	case EPageScanRepetitionModeChangeEvent:
	case EUserPasskeyRequestEvent:
		LOG1(_L("Warning!! Unhandled Command Event (event code:%d)"), aEvent.EventCode());
		break;
		
	default:
		LOG1(_L("Warning!! Unknown Command Event Received (event code:%d)"), aEvent.EventCode());
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownCommandEvent));
		break;
		}
	}

void CHCIFacade::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand)
	{
	LOG_FUNC
	LOG1(_L("MhcqcCommandErrored: error code:%d"), aErrorCode);

	// The stack maybe in a state that requires a response from the errored command so
	// fake up a command status event. This will be correctly processed by the stack even if the 
	// errored command was not expecting a command status event.
	if (aCommand != NULL)
		{
		LOG1(_L("... associated Command (opcode=%d)"), aCommand->Opcode());

		THCIErrorCode err(ECommandDisallowed);		
		if ((aErrorCode < KHCIErrorBase) && (aErrorCode > (KHCIErrorBase - static_cast<TInt>(KMaxTUint8)))) // error code is 8bits
			{
			// aErrorCode is an HCI Error, use it to inform stack
			err = (THCIErrorCode) (KHCIErrorBase - aErrorCode);
			}
			
		if (iInitState != EInitialised)
			{
			__ASSERT_ALWAYS(iOutstandingCommands.Count(), Panic(EHCICtrlrInitNoOutstandingCmds));
			iInitialisationError = ETrue;
			}
		
		TBuf8<256> fakeCommandStatusBuffer;
		TCommandStatusEvent fakeCommandStatusEvent(err, 1, aCommand->Opcode(), fakeCommandStatusBuffer);
		MhcqcCommandEventReceived(fakeCommandStatusEvent, aCommand);
		
		}
	}

// MHCIDataEventObserver
void CHCIFacade::MhdeoEventNotification(THCIEventBase& aEvent)
	{
	LOG_FUNC
	switch(aEvent.EventCode())
		{
	case ENumberOfCompletedPacketsEvent:
		NumberOfCompletedPacketsEvent(aEvent);
		break;
		
	case EFlushOccurredEvent:
		LOG(_L("Link [HCIFacade_Events.cpp]: FlushOccuredEvent - NOT HANDLED"));
		break;
		
	case EDataBufferOverflowEvent:
		Panic(ELinkMgrOverflowedHostController);  // this is really bad - out HC modelling has gone wrong...
		break;
		
	case EQOSViolationEvent:
		LOG(_L("Link [HCIFacade_Events.cpp]: Warning!! Unhandled QOSViolationEvent"));
		// we don't do QOS at the moment -drop
		break;
		
	default:
		LOG(_L("HCIFacade: Warning!! Unknown Data Event Received"));
		__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownDataEvent));
		break;
		}
	}


