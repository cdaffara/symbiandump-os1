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
// Interface to the HCI Facade, which hides the HCI level
// of the stack
// 
//

#ifndef HCIFACADE_H
#define HCIFACADE_H

#include "secman.h"
#include "linkmgr.h"
#include "notification.h"
#include "physicallinksmanager.h"

#include <btmanclient.h>
#include <es_mbuf.h>
#include <bluetooth/hardresetinitiator.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/hcicmdqcontroller.h>
#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/physicallinksstate.h>
#include <bluetooth/hci/controllerstateobserver.h>
#include <bluetooth/hci/controllerinitialisationobserver.h>
#include <bluetooth/hci/corehciplugin.h>
#include <bluetooth/hci/corehci.h>
#include <bluetooth/hci/hctlinterface.h>
#include <bluetooth/hci/controllerinitialisationinterface.h>
#include <bluetooth/hci/hcidataframer.h>
#include <bluetooth/hci/hcidataeventobserver.h>
#include <bluetooth/hci/hcidataobserver.h>
#include <bluetooth/hci/hcicommandallocator.h>
#include <bluetooth/hci/basebandpacketconsts.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hciclientusage.h>

class CACLDataQController;
class CHCICommandItem;
class CHCIDirectAccessSAPsManager;
class CHCICmdQController;

// low power mode constants - these are the stack preferred values
// but an HCI can ignore the parameter given by the stack and format
// a command with different values if it wishes
// this would allow for values that the local Controller prefers
static const TBasebandTime	KBTParkModeBeaconMaxInterval = 0x1000;
static const TBasebandTime	KBTParkModeBeaconMinInterval = 0x0400;

static const TBasebandTime	KBTHoldModeMaxInterval = 0x4000;
static const TBasebandTime	KBTHoldModeMinInterval = 0x040;

static const TBasebandTime	KBTSniffModeMaxInterval = 0x0540;
static const TBasebandTime	KBTSniffModeMinInterval = 0x0320;
static const TBasebandTime	KBTSniffModeAttempt		= 0x0008;
static const TBasebandTime	KBTSniffModeTimeout		= 0x0004;

static const TUint32 KNoEDReSCOPacketMask = 0x3C0;

static const TUint8 KRemoteExtendedFeaturesPage1 = 1;

// voice settings - see above: the HCI can modify this value as it sees fit
// but at least the stack gives it a chance by calling HCI in InitL
static const TUint16		KBTVoiceSetting			= 0x0000;

// max latency settings for eSCO connection
static const TUint16 		KeSCOMaxLatencyDontCare	= 0xffff;

NONSHARABLE_CLASS(TBasebandPolicy) : private TBasebandPolicyParams
	{
public:
	inline void InitialPolicy(const TBasebandPolicyParams& aParams);
	TInt TryToChangePolicy(const TBasebandPolicyParams& aNewParams);
	inline TBasebandPageTimePolicy PageTimePolicy() const;

	static const TBasebandTime KPageTimeoutR0;
	static const TBasebandTime KPageTimeoutR1;
	static const TBasebandTime KPageTimeoutR2;

	static const TReal KBestEffortTimeMultiplier;
	static const TReal KQuickTimeMultiplier;

private:
	};



NONSHARABLE_CLASS(CAFHTimer) : public CTimer
	{
public:
	static CAFHTimer* NewL(CHCIFacade& aParent);
	void SetPending(const TBTAFHHostChannelClassification& aHCC);
	void Reset();

private:
	void ConstructL();
	CAFHTimer(CHCIFacade& aParent);
	void RunL();
	TInt RunError(TInt aError);
	void RemovePendingHostChannelClassifications();
	//NB DoCancel not needed: version in CTimer does what we want..
	//...so no destructor (calling Cancel()) needed either!

private:
	CHCIFacade&	iParent;
	TBTAFHHostChannelClassification iHCC;
	TBool iPending;
	};



/**
	(L2CAP) HCI Link manager.
	This class interfaces to the HCI.  It's responsible for creating
	and destroying links, and passing commands from the l2cap level to
	the HCI.
	It is the one and only client of CHCI, and is responsible for loading
	the library that it lives in.

	It also passes on notifications that it receives from the HCI.
**/

NONSHARABLE_CLASS(CHCIFacade) : public CBase, public MPINCodeResponseHandler,
								public MLinkKeyResponseHandler,
								public MHardResetInitiator,
								public MControllerStateObserver,
								public MControllerInitialisationObserver,
								public MHCICommandQueueClient,
								public MHCIDataEventObserver,
								public MHCIDataObserver,
								public MHCTLChannelObserver,
								public MPhysicalLinksState,
								public MLinkMuxNotifier,
								public MHCIClientUsage
	{
#ifdef _DEBUG
friend class CACLDataQController; // for updating HC model
#endif

public:
	// Construction
	static CHCIFacade* NewL(CLinkMgrProtocol& aProtocol);
	void InitL(const TBTLocalDevice& aLocalDevice);
	~CHCIFacade();
	
	void SetLinkMuxer(CLinkMuxer& aLinkMuxer);
	CHCICmdQController& CommandQController() const {return *iCmdController;};

	CHctlAclDataFrame* NewACLDataFrameL(TUint16 aSize) const;
#ifdef STACK_SCO_DATA
	CHctlSynchronousDataFrame* NewSynchronousDataFrameL(TUint8 aSCODataLen) const;
#endif


	TInt WriteACLData(const CHctlAclDataFrame& aFrame) const;
	TInt WriteSCOData(const CHctlSynchronousDataFrame& aFrame) const;

	void FormatACLData(CHctlAclDataFrame& aFrame, THCIConnHandle aHandle,
					   TUint8 aOptions, const TDesC8& aData) const;
	void FormatSCOData(CHctlSynchronousDataFrame& aFrame, THCIConnHandle aHandle,
					   const TDesC8& aData) const;

	THCITransportChannel HCTLState() const;
	void SetPhysicalLinksMgr(CPhysicalLinksManager& aConnectionsMgr);

	// HCI commands
	void ConnectL(const TBTDevAddr& aAddr, TUint16 aPacketType=KDM1Packet, TUint8 aPageScanRepetitionMode=0,
			      TUint8 aPageScanMode=0, TUint16 aClockOffset=0, TUint8 aAllowRoleSwitch=1);
	void SCOConnectL(THCIConnHandle aACLHandle, TUint16 aPacketTypeMask, const TBTDevAddr& aBdaddr);
	void SetupSynchronousConnectionCommandL(THCIConnHandle aACLHandle, TUint aTransmitBandwidth, TUint aReceiveBandwidth,
			TUint16 aMaxLatency, TUint16 aVoiceSettings, TUint8	 aRetransmissionEffort,
			TBTSyncPacketTypes aPacketTypeMask, const TBTDevAddr& aBdaddr);
	void AcceptSynchronousConnectionL(const TBTDevAddr& aBdaddr, TUint aTransmitBandwidth, TUint aReceiveBandwidth,
			TUint16 aMaxLatency, TUint16 aContentFormat, TUint8	 aRetransmissionEffort,
			TBTSyncPacketTypes aPacketTypeMask);
	void RejectSynchronousConnectionL(const TBTDevAddr& aBdaddr, THCIErrorCode aReason);
	void CompleteConnectHandleError(const TBTDevAddr& aAddr, TBool JuiceFound, TInquiryLogEntry& juice) const;	//Called by async device with new juice info
	void DisconnectL(THCIConnHandle aConnH, THCIErrorCode aErr);
	void WriteScanEnableL(THCIScanEnable aEnable);
	
	void AcceptConnectionRequestL(const TBTDevAddr& aAddr, TUint8 aRole);
	void RejectConnectionRequestL(const TBTConnect& aConnect, THCIErrorCode aReason);
	void WriteLinkPolicySettingsL(THCIConnHandle aConnH, TUint16 aSettings);
	void FlushL(THCIConnHandle aConnH);
	void SetEventMaskL(const THCIEventMask& aEventMask);

	void RefreshEncryptionKeyL(THCIConnHandle aHandle);
	void WriteLinkSupervisionTimeoutL(THCIConnHandle aHandle, TInt aTimeout);

	void WriteLocalNameL(const TDesC8& aName);
	void WriteDeviceClassL(TUint aCoD);
	void WriteAFHChannelAssessmentModeL(TBool aEnable);
	void SetAFHHostChannelClassificationL(const TBTAFHHostChannelClassification& aChannelClassification);

	void VendorSpecificCommandL(const THCIOpcode aOpcode, const TDesC8& aBuffer);

	// from MLinkKeyResponseHandler
	TInt LinkKeyRequestReply(const TBTDevAddr& aBdaddr,const TDesC8& aLinkKey) const;
	TInt LinkKeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const;

	// from MPINCodeResponseHandler
	TInt PINCodeRequestReply(const TBTDevAddr& aBdaddr,const TDesC8& aPIN) const; 
	TInt PINCodeRequestNegativeReply(const TBTDevAddr& aBdaddr) const;

	TInt ChangeConnectionPacketType(THCIConnHandle aConnHandle, TUint16 aType);
	void AuthenticateL(THCIConnHandle aConnHandle);
	TInt ChangeConnectionLinkKey(THCIConnHandle aConnHandle);
	void SetEncryptL(THCIConnHandle aConnHandle,TBool aEnable);
	void ReadRemoteVersionL(THCIConnHandle aHandle);
	void ReadRemoteSupportedFeaturesL(THCIConnHandle aHandle);
	void ReadRemoteExtendedFeaturesL(THCIConnHandle aHandle, TUint8 aPageNumber);
	void ReadClockOffsetL(THCIConnHandle aHandle);
	void WriteIACLAPL(TUint8 aNumCurrentIAC, TUint aIAC_LAP[]);

	// from MControllerInitialisationObserver
	void McioPreResetCommandComplete(TInt aError);
	void McioPostResetCommandComplete(TInt aError);

	// from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);

	// from MControllerStateObserver
	void McsoProcessPowerChange(TInt aError, TControllerChangeType aChangeType, TBTPowerState aState);
	void McsoProcessHardResetPhaseChange(TInt aError, TControllerChangeType aChangeType, TBTHardResetState aState);

	// from MHardResetInitiator
	void MhriStartHardReset();

	// from MHCIDataEventObserver
	void MhdeoEventNotification(THCIEventBase& aEvent);
	
	// from MHCIDataObserver
	void MhdoProcessAclData(THCIConnHandle aConnH, TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, const TDesC8& aData);
	void MhdoProcessSynchronousData(THCIConnHandle aConnH, TUint8 aReserved, const TDesC8& aData);
	
	// from MHCTLChannelObserver
	void MhcoChannelOpen(THCITransportChannel aChannels);
	void MhcoChannelClosed(THCITransportChannel aChannels);

	// from MPhysicalLinksState - Simply forward the request to iLinksMgr
	TInt MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType) const;
	TInt MplsGetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType) const;
	TInt MplsGetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const;
	TInt MplsGetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const;
	TInt MplsGetRemoteAddress(TBTDevAddr& aBDAddr, THCIConnHandle aConnectionHandle) const;
	TInt MplsGetRemoteDeviceClass(TBTDeviceClass& aDeviceClass, const TBTDevAddr& aBDAddr) const;
	TInt MplsGetRemoteSupportedFeatures(TBTFeatures& aRemoteSupportedFeatures, const TBTDevAddr& aBDAddr) const;
	TInt MplsGetLinkPolicySettings(TLinkPolicy& aLinkPolicySettings, const TBTDevAddr& aBDAddr) const;
	TInt MplsGetBasebandLinkState(TBTBasebandLinkState& aBasebandLinkState, const TBTDevAddr& aBDAddr) const;

	// from MLinkMuxNotifier - Simply forward the request to iLinkMuxer
	void TryToSend();

	// from MHCIClientUsage
	void MhcuOpenClientReference();
	void MhcuCloseClientReference();

	TInt ChangeMode(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	void DoChangeModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	TInt ExitMode(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	void DoExitModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle);

	void ChangeRoleL(TBTBasebandRole aRole, const TBTDevAddr& aAddr);

	void SetControllerToHostFlowControlL();
	void AdvertiseHostBufferSizeL(TUint16 aNumBuffers);
	void ObtainHostControllerBufferSizeL();

	// simple forwarders
	TUint16 ReadACLReportingInterval() const;
	TUint16 ReadACLFramingOverhead() const;
	
	void ReadDeviceClassL();
private:
	TInt SendInitialisationCommand(CHCICommandBase* aCommand);
	void DoSendPostResetCommandsL();
	
	// Event processing functions
	void ConnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void AuthenticationCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadRemSuppFeatCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadRemoteExtendedFeaturesCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadRemVerInfoCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void CommandCompleteEvent(THCIOpcode aOpcode, THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void CommandStatusEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void FlushOccurredEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadClockOffsetEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ConnectionPacketTypeChangedEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void LinkSupervisionTimeoutChangedEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void SynchronousConnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void VendorDebugEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ConnectionRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void DisconnectionCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void EncryptionChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void HardwareErrorEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void RoleChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void ModeChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void PINCodeRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void LinkKeyRequestEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void LinkKeyNotificationEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MaxSlotsChangeEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void EncryptionKeyRefreshCompleteEvent(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);

	// Data event processing functions
	void NumberOfCompletedPacketsEvent(THCIEventBase& aEvent);

	// Command Complete processing functions
	void WriteLinkPolicySettingsOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ResetOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadBdaddrOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadBufferSizeOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadClassOfDeviceOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void WriteLocalNameOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void WriteCurrentIACLAPOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void WriteClassOfDeviceOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void SetControllerToHostFlowControlOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void HostBufferSizeOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void WriteScanEnableOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void SetAFHHostChannelClassificationOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void WriteAFHChannelAssessmentModeOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadLocalVersionInfoOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadLocalSupportedFeaturesOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadLocalSupportedCommandsOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void FlushOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void SwitchRoleOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void SetEventMaskOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	void ReadInquiryResponseTransmitPowerLevelOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);
	
	void WriteSimplePairingModeOpcode(THCIErrorCode aHciErr, const THCIEventBase* aEvent, const CHCICommandBase* aRelatedCommand);	
	
	// Functions
	CHCIFacade(CLinkMgrProtocol& aProtocol);
	void ConstructL();

	void HandlePowerStatusChange(TBTPowerState aStatus);

	void SniffL(THCIConnHandle aHandleToRemote);
	void ExitSniffL(THCIConnHandle aHandleToRemote);
	void HoldL(THCIConnHandle aHandle);
	void ParkL(THCIConnHandle aHandleToRemote);
	void ExitParkL(THCIConnHandle aHandleToRemote);

	void SetupFlowControlL(TFlowControlMode aMode);
	
	// Helper methods.	These are used to hide the issue
	// of having an RArray that stores 2 byte THCIOpcode values.
	TInt AddOutstandingCommandOpCode(THCIOpcode aOpCode);
	TInt FindOutstandingCommandOpCode(THCIOpcode aOpCode) const;
	
	// This function is used to Set Supported events mask then call SetEventMaskL
	void SetSupportedEventMasks();
	
private:
	// Data
	CLinkMgrProtocol&				iLinkMgrProtocol;
	CPhysicalLinksManager*			iLinksMgr;
	CHCIDirectAccessSAPsManager*	iHCIDirectAccessSAPsMgr;

	CLinkMuxer*				iLinkMuxer; // non-owned

	// HCI CommandQ
	CHCICmdQController*	iCmdController;		// we put things on the command Q
	
	enum TInitState
		{
		EIdle,
		EPreReset,
		EResetting,
		EReset,
		EPostReset,
		EInitialised,
		};
	
	TBool iReadLocalSupportedFeaturesComplete;
	TBool iReadLocalVersionComplete;

	
	TInitState iInitState;
	
	// HCI utility library
	CHciUtil* iHciUtil;
	
	// Core HCI and associated interfaces
	CCoreHCIPlugin*										iCoreHciPlugin;
	MCoreHci*											iCoreHci;
	MHCTLInterface*										iHctl;
	MControllerInitialisationInterface*					iControllerInitialisor;
	MControllerInitialisationAbortExtensionInterface* 	iControllerInitAbortExtension;
	MHCIDataFramer*										iDataFramer;
	MHardResetInitiator*								iHardResetInitiator;
	
	// Timer for required interval between AFH Host Channel Classification commands
	CAFHTimer*				iAFHTimer;

	// Data
	THCITransportChannel	iHCTLState; // memorize the status of the free channels

	TBTPowerState        iLastPowerState;

	// This array contains outstanding command Op Codes.  The array is 
	// not defined using THCIOpcode as the template type because RArray
	// types must be word aligned.
	RArray<TUint32> iOutstandingCommands;
	TUint32 iDeviceClass;
	TBool iInitialisationError;
	TInt iTxPowerLevel;
	};

#endif
