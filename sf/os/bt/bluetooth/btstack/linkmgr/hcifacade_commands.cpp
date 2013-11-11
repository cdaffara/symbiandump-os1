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
// These functions are called by parts of the stack that would like to issue
// HCI commands.
// The general pattern is for these functions create a new command and then
// add it to the command Q, which takes ownership of the command.
// 
//

#include <bluetooth/logger.h>
#include "hcifacade.h"
#include "linkutil.h"
#include "linkconsts.h"
#include "hostresolver.h"

#include <e32std.h>
#include <bt_sock.h>
#include "debug.h"
#include <utf.h>
#include <es_ini.h>
#include <btextnotifiers.h>

#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hcicommandqitem.h>

// Command Headers
#include <bluetooth/hci/writescanenablecommand.h>
#include <bluetooth/hci/createaclconnectioncommand.h>
#include <bluetooth/hci/setupsynchronousconnectioncommand.h>
#include <bluetooth/hci/acceptsynchronousconnectionrequestcommand.h>
#include <bluetooth/hci/rejectsynchronousconnectionrequestcommand.h>
#include <bluetooth/hci/disconnectcommand.h>
#include <bluetooth/hci/writelinkpolicysettingscommand.h>
#include <bluetooth/hci/acceptconnectionrequestcommand.h>
#include <bluetooth/hci/rejectconnectionrequestcommand.h>
#include <bluetooth/hci/readremoteversioninfocommand.h>
#include <bluetooth/hci/readremotesupportedfeaturescommand.h>
#include <bluetooth/hci/readremoteextendedfeaturescommand.h>
#include <bluetooth/hci/readclockoffsetcommand.h>
#include <bluetooth/hci/remotenamerequestcommand.h>
#include <bluetooth/hci/flushcommand.h>
#include <bluetooth/hci/seteventmaskcommand.h>
#include <bluetooth/hci/writelocalnamecommand.h>
#include <bluetooth/hci/linkkeyrequestreplycommand.h>
#include <bluetooth/hci/linkkeyrequestreplynegativecommand.h>
#include <bluetooth/hci/changelinkkeycommand.h>
#include <bluetooth/hci/pincoderequestreplycommand.h>
#include <bluetooth/hci/pincoderequestreplynegativecommand.h>
#include <bluetooth/hci/changeconnectionpackettypecommand.h>
#include <bluetooth/hci/authenticationrequestedcommand.h>
#include <bluetooth/hci/setconnectionencryptioncommand.h>
#include <bluetooth/hci/sniffmodecommand.h>
#include <bluetooth/hci/exitsniffmodecommand.h>
#include <bluetooth/hci/holdmodecommand.h>
#include <bluetooth/hci/parkmodecommand.h>
#include <bluetooth/hci/exitparkmodecommand.h>
#include <bluetooth/hci/switchrolecommand.h>
#include <bluetooth/hci/writeclassofdevicecommand.h>
#include <bluetooth/hci/setafhhostchannelclassificationcommand.h>
#include <bluetooth/hci/writeafhchannelassessmentmodecommand.h>
#include <bluetooth/hci/setcontrollertohostflowcontrolcommand.h>
#include <bluetooth/hci/readclassofdevicecommand.h>
#include <bluetooth/hci/readbuffersizecommand.h>
#include <bluetooth/hci/hostbuffersizecommand.h>
#include <bluetooth/hci/writecurrentiaclapcommand.h>
#include <bluetooth/hci/vendordebugcommand.h>
#include <bluetooth/hci/addscoconnectioncommand.h>
#include <bluetooth/hci/remotenamerequestcancelcommand.h>
#include <bluetooth/hci/refreshencryptionkeycommand.h>
#include <bluetooth/hci/writelinksupervisiontimeoutcommand.h>


void CHCIFacade::SCOConnectL(THCIConnHandle aACLHandle, TUint16 aPacketTypeMask, const TBTDevAddr& aBdaddr)
/**
	@pre Callers should check that the ACL Handle exists

	Attempts to attach an SCO link to the baseband
**/
	{
 	TBTDevHCIVersion hciVersion = iLinkMgrProtocol.GetHWHCIVersion(); // Read Hardware HCI Version
 	if (hciVersion.iHCIVersion > EHWHCIv1_1) // May not support Add_SCO_Connection
 		{
		#pragma message("KBTVoiceSetting - hardware specific value. KBTVoiceSetting should be replaced by HCI implementation, and should NOT be modified in a BT stack.")
		SetupSynchronousConnectionCommandL(aACLHandle,KBTSync64KBit,KBTSync64KBit,KeSCOMaxLatencyDontCare,KBTVoiceSetting,EeSCORetransmitDontCare,(aPacketTypeMask >> KSCOvsSyncHVOffset) | KNoEDReSCOPacketMask, aBdaddr); // Need to specify not to use EDR eSCO packets, as they are enabled by default
 		}
 	else // Supports Add_SCO_Connection
 		{
		CAddSCOConnectionCommand* cmd = CAddSCOConnectionCommand::NewL(aACLHandle, aPacketTypeMask, aBdaddr);
	
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		iCmdController->MhcqAddCommandL(cmd, *this);
 		}
	}

void CHCIFacade::SetupSynchronousConnectionCommandL(THCIConnHandle aACLHandle,
						TUint aTransmitBandwidth,	TUint aReceiveBandwidth, TUint16 aMaxLatency,
						TUint16 aVoiceSettings,	TUint8	aRetransmissionEffort,
						TBTSyncPacketTypes aPacketTypeMask, const TBTDevAddr& aBdaddr)
/**
	@pre Callers should check that the ACL Handle exists

	Attempts to attach a synchronous link to the baseband
**/
	{
	CSetupSynchronousConnectionCommand* cmd = CSetupSynchronousConnectionCommand::NewL(aACLHandle, aTransmitBandwidth, aReceiveBandwidth, aMaxLatency, aVoiceSettings, aRetransmissionEffort, aPacketTypeMask, aBdaddr);
	#pragma message("NB. This message relates to the CommandsEvents of the HCI(v2). It appears here because CE source code is generated.")
	#pragma message("Value of iVoiceSetting (see HCI Command) member is closely related to hardware used. ")
	#pragma message("Bits: Input Coding, Input Data, Input Sample Size and Linear_PCM_Bit_pos, should be set...")
	#pragma message("...according to hardware capabilities, and OR-ed to create appropriate iVoiceSetting value.")
	
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::AcceptSynchronousConnectionL(const TBTDevAddr& aBdaddr,
						TUint aTransmitBandwidth,	TUint aReceiveBandwidth, TUint16 aMaxLatency,
						TUint16 aContentFormat, TUint8	aRetransmissionEffort, TBTSyncPacketTypes aPacketTypeMask)
	{
	CAcceptSynchronousConnectionRequestCommand* cmd = CAcceptSynchronousConnectionRequestCommand::NewL(aBdaddr, aTransmitBandwidth, aReceiveBandwidth, aMaxLatency, aContentFormat, aRetransmissionEffort, aPacketTypeMask);
	
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}


void CHCIFacade::RejectSynchronousConnectionL(const TBTDevAddr& aBdaddr, THCIErrorCode aReason)
	{
	CRejectSynchronousConnectionRequestCommand* cmd = CRejectSynchronousConnectionRequestCommand::NewL(aBdaddr, aReason);
	
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}


void CHCIFacade::ConnectL(const TBTDevAddr& aAddr,
						 TUint16 aPacketType,
						 TUint8 aPageScanRepetitionMode,
						 TUint8 aPageScanMode,
						 TUint16 aClockOffset,
						 TUint8 aAllowRoleSwitch)
	{
	CCreateACLConnectionCommand* cmd = CCreateACLConnectionCommand::NewL(aAddr, aPacketType, aPageScanRepetitionMode, aPageScanMode, aClockOffset, aAllowRoleSwitch);
	
	// Before the CreateConnection command can be added to the queue we try
	// to ensure any inquiry is cancelled as some hardware will disallow the
	// connect otherwise.						
	iLinkMgrProtocol.InquiryMgr().Suspend();

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::DisconnectL(THCIConnHandle aConnH, THCIErrorCode aErr)
	{
	CDisconnectCommand* cmd = CDisconnectCommand::NewL(aConnH, aErr);
	
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteLinkPolicySettingsL(THCIConnHandle aConnH, TUint16 aSettings)
	{
	CWriteLinkPolicySettingsCommand* cmd = CWriteLinkPolicySettingsCommand::NewL(aConnH, aSettings);
	
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteScanEnableL(THCIScanEnable aEnable)
	{
	CWriteScanEnableCommand* cmd = CWriteScanEnableCommand::NewL(aEnable);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::AcceptConnectionRequestL(const TBTDevAddr& aAddr, TUint8 aRole)
	{
	CAcceptConnectionRequestCommand* cmd = CAcceptConnectionRequestCommand::NewL(aAddr, aRole);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::RejectConnectionRequestL(const TBTConnect& aConnect, THCIErrorCode aReason)
	{
	CRejectConnectionRequestCommand* cmd = CRejectConnectionRequestCommand::NewL(aConnect.iBdaddr, aReason);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}


void CHCIFacade::ReadRemoteVersionL(THCIConnHandle aHandle)
	{
	CReadRemoteVersionInfoCommand* cmd = CReadRemoteVersionInfoCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ReadRemoteSupportedFeaturesL(THCIConnHandle aHandle)
	{
	CReadRemoteSupportedFeaturesCommand* cmd = CReadRemoteSupportedFeaturesCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ReadRemoteExtendedFeaturesL(THCIConnHandle aHandle, TUint8 aPageNumber)
	{
	TInt err = KErrNotSupported;
	TBTDevHCIVersion hciVersion = iLinkMgrProtocol.GetHWHCIVersion(); // Read Hardware HCI Version
 	if (hciVersion.iHCIVersion > EHWHCIv1_1) // May not support Read_Remote_Extended_Features command
		{
		// Maybe migrate this to use HCIv2 so command is owned by physical link
		CReadRemoteExtendedFeaturesCommand* cmd = NULL;
		TRAP(err, cmd = CReadRemoteExtendedFeaturesCommand::NewL(aHandle, aPageNumber));
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		if(err == KErrNone)
			{
			TRAP(err, iCmdController->MhcqAddCommandL(cmd, *this));
			}
		}
	if(err != KErrNone)
		{
		// Some error occured but we have to signal the SSP state
		iLinksMgr->ReadRemoteExtendedFeaturesComplete(EUnknownOpcode, aHandle, TUint64(0), KRemoteExtendedFeaturesPage1, 0);
		User::Leave(err);
		}
	}


void CHCIFacade::ReadClockOffsetL(THCIConnHandle aHandle)
/**
	Create a command to find remote clock offset
**/
	{
	CReadClockOffsetCommand* cmd = CReadClockOffsetCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::RefreshEncryptionKeyL(THCIConnHandle aHandle)
	{
	CRefreshEncryptionKeyCommand* cmd = CRefreshEncryptionKeyCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteLinkSupervisionTimeoutL(THCIConnHandle aHandle, TInt aTimeout)
	{
	CWriteLinkSupervisionTimeoutCommand* cmd = CWriteLinkSupervisionTimeoutCommand::NewL(aHandle, aTimeout);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::FlushL(THCIConnHandle aConnH)
	{
	CFlushCommand* cmd = CFlushCommand::NewL(aConnH);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::SetEventMaskL(const THCIEventMask& aEventMask)
	{
	static const TUint8 KSizeOf64BitsInBytes = 8;
	TBuf8<KSizeOf64BitsInBytes> eventMask;
	eventMask.SetLength(KSizeOf64BitsInBytes);
	TUint8* ptr = const_cast<TUint8*>(eventMask.Ptr());
	LittleEndian::Put32(ptr, aEventMask.iLoBytes);
	LittleEndian::Put32(ptr+sizeof(TUint32), aEventMask.iHiBytes);
	
	CSetEventMaskCommand* cmd = CSetEventMaskCommand::NewL(eventMask);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteLocalNameL(const TDesC8& aName)
	{
	CWriteLocalNameCommand* cmd = CWriteLocalNameCommand::NewL(aName);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

TInt CHCIFacade::LinkKeyRequestReply(const TBTDevAddr& aBdaddr, const TDesC8& aLinkKey) const
/**
	We have obtained a linkkey, tell HC
**/
	{
	CLinkKeyRequestReplyCommand* cmd = NULL;
	TBTLinkKey linkkey;
	linkkey.Copy(aLinkKey);

	TRAPD(err, cmd = CLinkKeyRequestReplyCommand::NewL(aBdaddr, linkkey));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, const_cast<CHCIFacade&>(*this)));
		}
	return err;
	}
	
TInt CHCIFacade::LinkKeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const
/**
	We have not obtained a linkkey, tell HC
**/
	{
	CLinkKeyRequestReplyNegativeCommand* cmd = NULL;

	TRAPD(err, cmd = CLinkKeyRequestReplyNegativeCommand::NewL(aBdaddr));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, const_cast<CHCIFacade&>(*this)));
		}
	return err;
	}
	
TInt CHCIFacade::ChangeConnectionLinkKey(THCIConnHandle aConnHandle)
/**
Change the link key for the specified connection handle
**/
	{
	CChangeLinkKeyCommand* cmd = NULL;

	TRAPD(err, cmd = CChangeLinkKeyCommand::NewL(aConnHandle));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, *this));
		}
	return err;
	}


TInt CHCIFacade::PINCodeRequestReply(const TBTDevAddr& aBdaddr,const TDesC8& aPIN) const
/**
	We have obtained a PIN, tell HC
**/
	{
	CPINCodeRequestReplyCommand* cmd = NULL;
	TBTPinCode pin;
	pin.Copy(aPIN);
	TPtrC8 pinContents(pin().iPIN, pin().iLength);

	TRAPD(err, cmd = CPINCodeRequestReplyCommand::NewL(aBdaddr, pin().iLength, pinContents));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, const_cast<CHCIFacade&>(*this)));
		}
	return err;
	}

TInt CHCIFacade::PINCodeRequestNegativeReply(const TBTDevAddr& aBdaddr) const
/**
	We have not obtained a PIN, tell HC
**/
	{
	CPINCodeRequestReplyNegativeCommand* cmd = NULL;
	
	TRAPD(err, cmd = CPINCodeRequestReplyNegativeCommand::NewL(aBdaddr));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, const_cast<CHCIFacade&>(*this)));
		}
	return err;
	}

TInt CHCIFacade::ChangeConnectionPacketType(THCIConnHandle aConnHandle, TUint16 aType)
	{
	CChangeConnectionPacketTypeCommand* cmd = NULL;

	TRAPD(err, cmd = CChangeConnectionPacketTypeCommand::NewL(aConnHandle, aType));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController->MhcqAddCommandL(cmd, *this));
		}
	return err;
	}

void CHCIFacade::AuthenticateL(THCIConnHandle aConnHandle)
/**
	Do an authentication
**/
	{
	CAuthenticationRequestedCommand* cmd = CAuthenticationRequestedCommand::NewL(aConnHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::SetEncryptL(THCIConnHandle aConnHandle,TBool aEnable)
/**
	Creates command to enable/disable encryption on a given link
**/
	{
	CSetConnectionEncryptionCommand* cmd = CSetConnectionEncryptionCommand::NewL(aConnHandle, aEnable);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::SniffL(THCIConnHandle aHandleToRemote)
	{
	CSniffModeCommand* cmd = CSniffModeCommand::NewL(aHandleToRemote, KBTSniffModeMaxInterval, KBTSniffModeMinInterval, KBTSniffModeAttempt, KBTSniffModeTimeout);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ExitSniffL(THCIConnHandle aHandleToRemote)
	{
	CExitSniffModeCommand* cmd = CExitSniffModeCommand::NewL(aHandleToRemote);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::HoldL(THCIConnHandle aHandle)
	{
	CHoldModeCommand* cmd = CHoldModeCommand::NewL(aHandle, KBTHoldModeMaxInterval, KBTHoldModeMinInterval);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ParkL(THCIConnHandle aHandle)
	{
	CParkModeCommand* cmd = CParkModeCommand::NewL(aHandle, KBTParkModeBeaconMaxInterval, KBTParkModeBeaconMinInterval);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ExitParkL(THCIConnHandle aHandle)
	{
	CExitParkModeCommand* cmd = CExitParkModeCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ChangeRoleL(TBTBasebandRole aRole, const TBTDevAddr& aAddr)
	{
	CSwitchRoleCommand* cmd = CSwitchRoleCommand::NewL(aAddr, aRole);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteDeviceClassL(TUint aCoD)
	{
	CWriteClassOfDeviceCommand* cmd = CWriteClassOfDeviceCommand::NewL(aCoD);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::SetAFHHostChannelClassificationL(const TBTAFHHostChannelClassification& aChannelClassification)
	{
	__ASSERT_DEBUG(aChannelClassification.Validate()==KErrNone, 
				   __BTDEBUGGER());
	if(aChannelClassification.Validate()!=KErrNone)
		{
		//NB If this is called by one of Bluetooth's "Publish
		//and Subscribe" subscribers, then the KErrNotSupported
		//will be dropped - no error will be reported to the user...
		//which is good - we do not want the user reacting to errors.
		User::Leave(KErrNotSupported);
		}
	if(iAFHTimer->IsActive())
		{
		//Timer is active => a host channel classification has just been sent.
		//Setup timer so that this command is postponed to occur more than one second after 
		//that last host channel classification command - as per spec.
		//NB You only need the latest Host Channel Classification command
		//when the timer fires.
		iAFHTimer->SetPending(aChannelClassification);
		}
	else
		{
		//Start AFHHost Channel Classification Command blocking timer with a 
		//long timeout (in case command never completes). 
		//This will be updated when/if the command does complete.
		iAFHTimer->After(KAFHHostChannelClassificationCommandCompleteTimer);

		CSetAFHHostChannelClassificationCommand* cmd = CSetAFHHostChannelClassificationCommand::NewL(aChannelClassification);

		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		iCmdController->MhcqAddCommandL(cmd, *this);
		}
	}

void CHCIFacade::WriteAFHChannelAssessmentModeL(TBool aEnable)
	{
	CWriteAFHChannelAssessmentModeCommand* cmd = CWriteAFHChannelAssessmentModeCommand::NewL(aEnable?1:0);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::SetControllerToHostFlowControlL()
	{
	CSetControllerToHostFlowControlCommand* cmd = CSetControllerToHostFlowControlCommand::NewL(ETrue);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::ReadDeviceClassL()
	{
	CReadClassOfDeviceCommand* cmd = CReadClassOfDeviceCommand::NewL();

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}


void CHCIFacade::ObtainHostControllerBufferSizeL()
	{
	CReadBufferSizeCommand* cmd = CReadBufferSizeCommand::NewL();

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::AdvertiseHostBufferSizeL(TUint16 aNumBuffers)
	{
	CHostBufferSizeCommand* cmd = CHostBufferSizeCommand::NewL(KLinkMgrIncomingBufferSize, KStackSCOBuffersSize, aNumBuffers, KStackSCOBuffersNum);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::WriteIACLAPL(TUint8 aNumCurrentIAC, TUint aIAC_LAP[])
	{
	RArray<TUint32> iacLap;
	
	for (TInt count = 0; count < aNumCurrentIAC; count++)
		{
		iacLap.AppendL(aIAC_LAP[count]);
		}
		
	CWriteCurrentIACLAPCommand* cmd = CWriteCurrentIACLAPCommand::NewL(aNumCurrentIAC, iacLap);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}

void CHCIFacade::VendorSpecificCommandL(const THCIOpcode aOpcode,
										const TDesC8& aParameters)
	{
	CVendorDebugCommand* cmd = CVendorDebugCommand::NewL(aOpcode);
	
	CleanupStack::PushL(cmd);
	if(aParameters.Length() > cmd->Command().MaxLength())
		{
		User::Leave(KErrOverflow);
		}
	CleanupStack::Pop(); // We don't leave from this point on.

 	cmd->Command() = aParameters;

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController->MhcqAddCommandL(cmd, *this);
	}
