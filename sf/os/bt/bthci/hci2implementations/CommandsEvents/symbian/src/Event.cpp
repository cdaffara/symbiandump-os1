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


#include <bluetooth/hci/event.h>
#include <bluetooth/hci/vendordebugevent.h>
#include <bluetooth/hci/vendordebugcompleteevent.h>
#include <bluetooth/hci/commandcompleteevent.h>

#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hcicommandeventobserver.h>
#include <bluetooth/hci/hcidataeventobserver.h>
#include <bluetooth/hci/hciconsts.h>

// Complete Event Class Headers
#include <bluetooth/hci/nopcompleteevent.h>
#include <bluetooth/hci/inquirycancelcompleteevent.h>
#include <bluetooth/hci/periodicinquirymodecompleteevent.h>
#include <bluetooth/hci/exitperiodicinquirymodecompleteevent.h>
#include <bluetooth/hci/createconnectioncancelcompleteevent.h>
#include <bluetooth/hci/linkkeyrequestreplycompleteevent.h>
#include <bluetooth/hci/linkkeyrequestreplynegativecompleteevent.h>
#include <bluetooth/hci/pincoderequestreplycompleteevent.h>
#include <bluetooth/hci/pincoderequestreplynegativecompleteevent.h>
#include <bluetooth/hci/remotenamerequestcancelcompleteevent.h>
#include <bluetooth/hci/readlmphandlecompleteevent.h>
#include <bluetooth/hci/qossetupcompleteevent.h>
#include <bluetooth/hci/rolediscoverycompleteevent.h>
#include <bluetooth/hci/readlinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/writelinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/readdefaultlinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/writedefaultlinkpolicysettingscompleteevent.h>
#include <bluetooth/hci/sniffsubratingcompleteevent.h>
#include <bluetooth/hci/flowspecificationcompleteevent.h>
#include <bluetooth/hci/seteventmaskcompleteevent.h>
#include <bluetooth/hci/resetcompleteevent.h>
#include <bluetooth/hci/seteventfiltercompleteevent.h>
#include <bluetooth/hci/flushcompleteevent.h>
#include <bluetooth/hci/readpintypecompleteevent.h>
#include <bluetooth/hci/writepintypecompleteevent.h>
#include <bluetooth/hci/createnewunitkeycompleteevent.h>
#include <bluetooth/hci/readstoredlinkkeycompleteevent.h>
#include <bluetooth/hci/writestoredlinkkeycompleteevent.h>
#include <bluetooth/hci/deletestoredlinkkeycompleteevent.h>
#include <bluetooth/hci/writelocalnamecompleteevent.h>
#include <bluetooth/hci/readlocalnamecompleteevent.h>
#include <bluetooth/hci/readconnectionaccepttimeoutcompleteevent.h>
#include <bluetooth/hci/writeconnectionaccepttimeoutcompleteevent.h>
#include <bluetooth/hci/readpagetimeoutcompleteevent.h>
#include <bluetooth/hci/writepagetimeoutcompleteevent.h>
#include <bluetooth/hci/readscanenablecompleteevent.h>
#include <bluetooth/hci/writescanenablecompleteevent.h>
#include <bluetooth/hci/readpagescanactivitycompleteevent.h>
#include <bluetooth/hci/writepagescanactivitycompleteevent.h>
#include <bluetooth/hci/readinquiryscanactivitycompleteevent.h>
#include <bluetooth/hci/writeinquiryscanactivitycompleteevent.h>
#include <bluetooth/hci/readauthenticationenablecompleteevent.h>
#include <bluetooth/hci/writeauthenticationenablecompleteevent.h>
#include <bluetooth/hci/readencryptionmodecompleteevent.h>
#include <bluetooth/hci/writeencryptionmodecompleteevent.h>
#include <bluetooth/hci/readclassofdevicecompleteevent.h>
#include <bluetooth/hci/writeclassofdevicecompleteevent.h>
#include <bluetooth/hci/readvoicesettingcompleteevent.h>
#include <bluetooth/hci/writevoicesettingcompleteevent.h>
#include <bluetooth/hci/readautomaticflushtimeoutcompleteevent.h>
#include <bluetooth/hci/writeautomaticflushtimeoutcompleteevent.h>
#include <bluetooth/hci/readnumbroadcastretransmissionscompleteevent.h>
#include <bluetooth/hci/writenumbroadcastretransmissionscompleteevent.h>
#include <bluetooth/hci/readholdmodeactivitycompleteevent.h>
#include <bluetooth/hci/writeholdmodeactivitycompleteevent.h>
#include <bluetooth/hci/readtransmitpowerlevelcompleteevent.h>
#include <bluetooth/hci/readsynchronousflowcontrolenablecompleteevent.h>
#include <bluetooth/hci/writesynchronousflowcontrolenablecompleteevent.h>
#include <bluetooth/hci/setcontrollertohostflowcontrolcompleteevent.h>
#include <bluetooth/hci/hostbuffersizecompleteevent.h>
#include <bluetooth/hci/hostnumberofcompletedpacketscompleteevent.h>
#include <bluetooth/hci/readlinksupervisiontimeoutcompleteevent.h>
#include <bluetooth/hci/writelinksupervisiontimeoutcompleteevent.h>
#include <bluetooth/hci/readnumberofsupportediaccompleteevent.h>
#include <bluetooth/hci/readcurrentiaclapcompleteevent.h>
#include <bluetooth/hci/writecurrentiaclapcompleteevent.h>
#include <bluetooth/hci/readpagescanperiodcompleteevent.h>
#include <bluetooth/hci/writepagescanperiodcompleteevent.h>
#include <bluetooth/hci/setafhhostchannelclassificationcompleteevent.h>
#include <bluetooth/hci/readinquiryscantypecompleteevent.h>
#include <bluetooth/hci/writeinquiryscantypecompleteevent.h>
#include <bluetooth/hci/readinquirymodecompleteevent.h>
#include <bluetooth/hci/writeinquirymodecompleteevent.h>
#include <bluetooth/hci/readpagescantypecompleteevent.h>
#include <bluetooth/hci/writepagescantypecompleteevent.h>
#include <bluetooth/hci/readafhchannelassessmentmodecompleteevent.h>
#include <bluetooth/hci/writeafhchannelassessmentmodecompleteevent.h>
#include <bluetooth/hci/readdefaulterroneousdatareportingcompleteevent.h>
#include <bluetooth/hci/writedefaulterroneousdatareportingcompleteevent.h>
#include <bluetooth/hci/readlocalversioninfocompleteevent.h>
#include <bluetooth/hci/readlocalsupportedcommandscompleteevent.h>
#include <bluetooth/hci/readlocalsupportedfeaturescompleteevent.h>
#include <bluetooth/hci/readlocalextendedfeaturescompleteevent.h>
#include <bluetooth/hci/readbuffersizecompleteevent.h>
#include <bluetooth/hci/readbdaddrcompleteevent.h>
#include <bluetooth/hci/readfailedcontactcountercompleteevent.h>
#include <bluetooth/hci/resetfailedcontactcountercompleteevent.h>
#include <bluetooth/hci/readlinkqualitycompleteevent.h>
#include <bluetooth/hci/readrssicompleteevent.h>
#include <bluetooth/hci/readafhchannelmapcompleteevent.h>
#include <bluetooth/hci/readclockcompleteevent.h>
#include <bluetooth/hci/readloopbackmodecompleteevent.h>
#include <bluetooth/hci/writeloopbackmodecompleteevent.h>
#include <bluetooth/hci/enabledeviceundertestmodecompleteevent.h>
#include <bluetooth/hci/readextendedinquiryresponsecompleteevent.h>
#include <bluetooth/hci/writeextendedinquiryresponsecompleteevent.h>
#include <bluetooth/hci/readinquiryresponsetransmitpowerlevelcompleteevent.h>
#include <bluetooth/hci/writeinquiryresponsetransmitpowerlevelcompleteevent.h>
#include <bluetooth/hci/iocapabilityrequestreplycompleteevent.h>
#include <bluetooth/hci/userconfirmationrequestreplycompleteevent.h>
#include <bluetooth/hci/userconfirmationrequestnegativereplycompleteevent.h>
#include <bluetooth/hci/userpasskeyrequestreplycompleteevent.h>
#include <bluetooth/hci/userpasskeyrequestnegativereplycompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestreplycompleteevent.h>
#include <bluetooth/hci/remoteoobdatarequestnegativereplycompleteevent.h>
#include <bluetooth/hci/iocapabilityrequestnegativereplycompleteevent.h>
#include <bluetooth/hci/readsimplepairingmodecompleteevent.h>
#include <bluetooth/hci/writesimplepairingmodecompleteevent.h>
#include <bluetooth/hci/readlocaloobdatacompleteevent.h>
#include <bluetooth/hci/sendkeypressnotificationcompleteevent.h>
#include <bluetooth/hci/writesimplepairingdebugmodecompleteevent.h>


// Event Class Headers
#include <bluetooth/hci/inquirycompleteevent.h>
#include <bluetooth/hci/inquiryresultevent.h>
#include <bluetooth/hci/connectioncompleteevent.h>
#include <bluetooth/hci/connectionrequestevent.h>
#include <bluetooth/hci/disconnectioncompleteevent.h>
#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/linkkeyrequestevent.h>
#include <bluetooth/hci/pincoderequestevent.h>
#include <bluetooth/hci/remotenamereqcompleteevent.h>
#include <bluetooth/hci/authenticationcompleteevent.h>
#include <bluetooth/hci/encryptionchangeevent.h>
#include <bluetooth/hci/changelinkkeyevent.h>
#include <bluetooth/hci/masterlinkkeyevent.h>
#include <bluetooth/hci/readremsuppfeatcompleteevent.h>
#include <bluetooth/hci/readremverinfocompleteevent.h>
#include <bluetooth/hci/hardwareerrorevent.h>
#include <bluetooth/hci/flushoccurredevent.h>
#include <bluetooth/hci/rolechangeevent.h>
#include <bluetooth/hci/numberofcompletedpacketsevent.h>
#include <bluetooth/hci/modechangeevent.h>
#include <bluetooth/hci/returnlinkkeysevent.h>
#include <bluetooth/hci/linkkeynotificationevent.h>
#include <bluetooth/hci/loopbackcommandevent.h>
#include <bluetooth/hci/databufferoverflowevent.h>
#include <bluetooth/hci/maxslotschangeevent.h>
#include <bluetooth/hci/readclockoffsetevent.h>
#include <bluetooth/hci/connectionpackettypechangedevent.h>
#include <bluetooth/hci/qosviolationevent.h>
#include <bluetooth/hci/pagescanrepetitionmodechangeevent.h>
#include <bluetooth/hci/inquiryresultwithrssievent.h>
#include <bluetooth/hci/readremoteextendedfeaturescompleteevent.h>
#include <bluetooth/hci/synchronousconnectioncompleteevent.h>
#include <bluetooth/hci/synchronousconnectionchangedevent.h>
#include <bluetooth/hci/sniffsubratingevent.h>
#include <bluetooth/hci/extendedinquiryresultevent.h>
#include <bluetooth/hci/encryptionkeyrefreshcompleteevent.h>
#include <bluetooth/hci/iocapabilityrequestevent.h>
#include <bluetooth/hci/iocapabilityresponseevent.h>
#include <bluetooth/hci/userconfirmationrequestevent.h>
#include <bluetooth/hci/userpasskeyrequestevent.h>
#include <bluetooth/hci/remoteoobdatarequestevent.h>
#include <bluetooth/hci/simplepairingcompleteevent.h>
#include <bluetooth/hci/linksupervisiontimeoutchangedevent.h>
#include <bluetooth/hci/enhancedflushcompleteevent.h>
#include <bluetooth/hci/userpasskeynotificationevent.h>
#include <bluetooth/hci/keypressnotificationevent.h>
#include <bluetooth/hci/remotehostsupportedfeaturesnotificationevent.h>
#include <bluetooth/hci/pagescanmodechangeevent.h>

#include "symbiancommandseventsutils.h"



#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_COMMANDSEVENTS_SYMBIAN);
#endif

THCIEventBase::THCIEventBase(THCIEventCode aEventCode, TUint8 aParameterTotalLength,
			TDes8& aEventData)
	: 
	iErrorCode(EOK)
	{
	PutEventCode(aEventCode, aEventData);			// Event code
	PutTUint8(aParameterTotalLength, aEventData);	// Parameter_Total_Length (in octets)
	
	// It is the responsibility of the most derived class to set iEventData
	// from aEventData after event-specific data has been added in.	
	}

EXPORT_C THCIEventBase::THCIEventBase(const TDesC8& aEventData)
	: 
	iErrorCode(EOK),
	iEventData(aEventData)
	{
	}

EXPORT_C THCIEventCode THCIEventBase::EventCode() const
	{
	return(static_cast<THCIEventCode>(iEventData[0]));	
	}
	
EXPORT_C THCIErrorCode THCIEventBase::ErrorCode() const
	{
	return(iErrorCode);	
	}

TInt THCIEventBase::CreateAndSendCommandCompleteEvent(const TDesC8& aEventData,
													  MHCICommandEventObserver& aEventObserver)
	{
	// Switch on Op Code.
	__ASSERT_ALWAYS(aEventData.Length() >= KEventCommonFieldsLength + THCICommandCompleteEvent::KCommandCompleteCommonFieldsLength,
				PANIC(KSymbianCommandsEventsPanicCat, EInvalidReceivedEventLength));
	THCIOpcode opcode((aEventData[4] << 8) | aEventData[3]);

	if ((opcode & KVendorDebugOGF) == KVendorDebugOGF)
		{
		// We don't know anything else about the opcode other than it
		// corresponds to a vendor command
		TVendorDebugCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		return KErrNone;
		}

	switch(opcode)
		{
	// Nop

	case KNopOpcode:
		{
		TNopCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;


	case KInquiryCancelOpcode:
		{
		TInquiryCancelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	case KPeriodicInquiryModeOpcode:
		{
		TPeriodicInquiryModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KExitPeriodicInquiryModeOpcode:
		{
		TExitPeriodicInquiryModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KCreateConnectionCancelOpcode:
		{
		TCreateConnectionCancelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KLinkKeyRequestReplyOpcode:
		{
		TLinkKeyRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KLinkKeyRequestReplyNegativeOpcode:
		{
		TLinkKeyRequestReplyNegativeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KPINCodeRequestReplyOpcode:
		{
		TPINCodeRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KPINCodeRequestReplyNegativeOpcode:
		{
		TPINCodeRequestReplyNegativeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KRemoteNameRequestCancelOpcode:
		{
		TRemoteNameRequestCancelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLMPHandleOpcode:
		{
		TReadLMPHandleCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KIOCapabilityRequestReplyOpcode:
		{
		TIOCapabilityRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KUserConfirmationRequestReplyOpcode:
		{
		TUserConfirmationRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KUserConfirmationRequestNegativeReplyOpcode:
		{
		TUserConfirmationRequestNegativeReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KUserPasskeyRequestReplyOpcode:
		{
		TUserPasskeyRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KUserPasskeyRequestNegativeReplyOpcode:
		{
		TUserPasskeyRequestNegativeReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KRemoteOOBDataRequestReplyOpcode:
		{
		TRemoteOOBDataRequestReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KRemoteOOBDataRequestNegativeReplyOpcode:
		{
		TRemoteOOBDataRequestNegativeReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KIOCapabilityRequestNegativeReplyOpcode:
		{
		TIOCapabilityRequestNegativeReplyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;


	// Link Policy

	case KRoleDiscoveryOpcode:
		{
		TRoleDiscoveryCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLinkPolicySettingsOpcode:
		{
		TReadLinkPolicySettingsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteLinkPolicySettingsOpcode:
		{
		TWriteLinkPolicySettingsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadDefaultLinkPolicySettingsOpcode:
		{
		TReadDefaultLinkPolicySettingsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteDefaultLinkPolicySettingsOpcode:
		{
		TWriteDefaultLinkPolicySettingsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	case KSniffSubratingOpcode:
		{
		TSniffSubratingCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;


	// Controller and Baseband

	case KSetEventMaskOpcode:
		{
		TSetEventMaskCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KResetOpcode:
		{
		TResetCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KSetEventFilterOpcode:
		{
		TSetEventFilterCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KFlushOpcode:
		{
		TFlushCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadPINTypeOpcode:
		{
		TReadPINTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWritePINTypeOpcode:
		{
		TWritePINTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KCreateNewUnitKeyOpcode:
		{
		TCreateNewUnitKeyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadStoredLinkKeyOpcode:
		{
		TReadStoredLinkKeyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteStoredLinkKeyOpcode:
		{
		TWriteStoredLinkKeyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KDeleteStoredLinkKeyOpcode:
		{
		TDeleteStoredLinkKeyCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteLocalNameOpcode:
		{
		TWriteLocalNameCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLocalNameOpcode:
		{
		TReadLocalNameCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadConnectionAcceptTimeoutOpcode:
		{
		TReadConnectionAcceptTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteConnectionAcceptTimeoutOpcode:
		{
		TWriteConnectionAcceptTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadPageTimeoutOpcode:
		{
		TReadPageTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWritePageTimeoutOpcode:
		{
		TWritePageTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadScanEnableOpcode:
		{
		TReadScanEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteScanEnableOpcode:
		{
		TWriteScanEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadPageScanActivityOpcode:
		{
		TReadPageScanActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWritePageScanActivityOpcode:
		{
		TWritePageScanActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadInquiryScanActivityOpcode:
		{
		TReadInquiryScanActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteInquiryScanActivityOpcode:
		{
		TWriteInquiryScanActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadAuthenticationEnableOpcode:
		{
		TReadAuthenticationEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteAuthenticationEnableOpcode:
		{
		TWriteAuthenticationEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadClassOfDeviceOpcode:
		{
		TReadClassOfDeviceCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteClassOfDeviceOpcode:
		{
		TWriteClassOfDeviceCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadVoiceSettingOpcode:
		{
		TReadVoiceSettingCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteVoiceSettingOpcode:
		{
		TWriteVoiceSettingCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadAutomaticFlushTimeoutOpcode:
		{
		TReadAutomaticFlushTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteAutomaticFlushTimeoutOpcode:
		{
		TWriteAutomaticFlushTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadNumBroadcastRetransmissionsOpcode:
		{
		TReadNumBroadcastRetransmissionsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteNumBroadcastRetransmissionsOpcode:
		{
		TWriteNumBroadcastRetransmissionsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadHoldModeActivityOpcode:
		{
		TReadHoldModeActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteHoldModeActivityOpcode:
		{
		TWriteHoldModeActivityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadTransmitPowerLevelOpcode:
		{
		TReadTransmitPowerLevelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadSynchronousFlowControlEnableOpcode:
		{
		TReadSynchronousFlowControlEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteSynchronousFlowControlEnableOpcode:
		{
		TWriteSynchronousFlowControlEnableCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KSetControllerToHostFlowControlOpcode:
		{
		TSetControllerToHostFlowControlCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KHostBufferSizeOpcode:
		{
		THostBufferSizeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KHostNumberOfCompletedPacketsOpcode:
		{
		THostNumberOfCompletedPacketsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLinkSupervisionTimeoutOpcode:
		{
		TReadLinkSupervisionTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteLinkSupervisionTimeoutOpcode:
		{
		TWriteLinkSupervisionTimeoutCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadNumberOfSupportedIACOpcode:
		{
		TReadNumberOfSupportedIACCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadCurrentIACLAPOpcode:
		{
		TReadCurrentIACLAPCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteCurrentIACLAPOpcode:
		{
		TWriteCurrentIACLAPCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KSetAFHHostChannelClassificationOpcode:
		{
		TSetAFHHostChannelClassificationCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadInquiryScanTypeOpcode:
		{
		TReadInquiryScanTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteInquiryScanTypeOpcode:
		{
		TWriteInquiryScanTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadInquiryModeOpcode:
		{
		TReadInquiryModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteInquiryModeOpcode:
		{
		TWriteInquiryModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadPageScanTypeOpcode:
		{
		TReadPageScanTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWritePageScanTypeOpcode:
		{
		TWritePageScanTypeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadAFHChannelAssessmentModeOpcode:
		{
		TReadAFHChannelAssessmentModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteAFHChannelAssessmentModeOpcode:
		{
		TWriteAFHChannelAssessmentModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadExtendedInquiryResponseOpcode:
		{
		TReadExtendedInquiryResponseCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteExtendedInquiryResponseOpcode:
		{
		TWriteExtendedInquiryResponseCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadSimplePairingModeOpcode:
		{
		TReadSimplePairingModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteSimplePairingModeOpcode:
		{
		TWriteSimplePairingModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLocalOOBDataOpcode:
		{
		TReadLocalOOBDataCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadInquiryResponseTransmitPowerLevelOpcode:
		{
		TReadInquiryResponseTransmitPowerLevelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteInquiryResponseTransmitPowerLevelOpcode:
		{
		TWriteInquiryResponseTransmitPowerLevelCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KSendKeypressNotificationOpcode:
		{
		TSendKeypressNotificationCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	case KReadDefaultErroneousDataReportingOpcode:
		{
		TReadDefaultErroneousDataReportingCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	case KWriteDefaultErroneousDataReportingOpcode:
		{
		TWriteDefaultErroneousDataReportingCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;


	// Informational Parameters

	case KReadLocalVersionInfoOpcode:
		{
		TReadLocalVersionInfoCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLocalSupportedCommandsOpcode:
		{
		TReadLocalSupportedCommandsCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLocalSupportedFeaturesOpcode:
		{
		TReadLocalSupportedFeaturesCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLocalExtendedFeaturesOpcode:
		{
		TReadLocalExtendedFeaturesCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadBufferSizeOpcode:
		{
		TReadBufferSizeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadBdaddrOpcode:
		{
		TReadBdaddrCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	
	// Status Parameters

	case KReadFailedContactCounterOpcode:
		{
		TReadFailedContactCounterCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KResetFailedContactCounterOpcode:
		{
		TResetFailedContactCounterCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadLinkQualityOpcode:
		{
		TReadLinkQualityCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadRSSIOpcode:
		{
		TReadRSSICompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadAFHChannelMapOpcode:
		{
		TReadAFHChannelMapCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadClockOpcode:
		{
		TReadClockCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
	

	// Testing Commmands

	case KReadLoopbackModeOpcode:
		{
		TReadLoopbackModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteLoopbackModeOpcode:
		{
		TWriteLoopbackModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KEnableDeviceUnderTestModeOpcode:
		{
		TEnableDeviceUnderTestModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteSimplePairingDebugModeOpcode:
		{
		TWriteSimplePairingDebugModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;
		
	// Deprecated (in the Bluetooth specification)

    case KReadPageScanPeriodOpcode:
		{
		TReadPageScanPeriodCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWritePageScanPeriodOpcode:
		{
		TWritePageScanPeriodCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KReadEncryptionModeOpcode:
		{
		TReadEncryptionModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	case KWriteEncryptionModeOpcode:
		{
		TWriteEncryptionModeCompleteEvent event(aEventData);
		aEventObserver.MhceoEventNotification(event);
		}
		break;

	// Unknown command complete event.
	default:
		return KErrNotFound;
		};

	return KErrNone;
	}

EXPORT_C /*static*/ TInt THCIEventBase::CreateAndSendEvent(const TDesC8& aEventData,
														   MHCICommandEventObserver& aEventObserver,
														   MHCIDataEventObserver& aDataEventObserver,
														   TAny* /*aObserver*/)
	{
	TInt rerr = KErrNone;

	if(aEventData.Length() >= KEventCommonFieldsLength 
	&& aEventData.Length() >= KEventCommonFieldsLength + aEventData[KTotalParameterLengthOffset])
		{
		// Switch on the event code.
		switch(aEventData[KEventCodeOffset])
			{
			case EInquiryCompleteEvent:
				{
				TInquiryCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
							
			case EInquiryResultEvent:
				{
				TInquiryResultEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
				
			case EConnectionCompleteEvent:
				{
				TConnectionCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
							
			case EConnectionRequestEvent:				
				{
				TConnectionRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EDisconnectionCompleteEvent:
				{
				TDisconnectionCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EAuthenticationCompleteEvent:
				{
				TAuthenticationCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;
					
			case ERemoteNameReqCompleteEvent:
				{
				TRemoteNameReqCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;
							
			case EEncryptionChangeEvent:
				{
				TEncryptionChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;
							
			case EChangeLinkKeyEvent:					
				{
				TChangeLinkKeyEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EMasterLinkKeyEvent:					
				{
				TMasterLinkKeyEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EReadRemSuppFeatCompleteEvent:		
				{
				TReadRemSuppFeatCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EReadRemVerInfoCompleteEvent:		
				{
				TReadRemVerInfoCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EQOSSetupCompleteEvent:
				{								
				TQOSSetupCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
				
			case ECommandCompleteEvent:				
				{
				rerr = CreateAndSendCommandCompleteEvent(aEventData,
														 aEventObserver);
				}
				break;
				
			case ECommandStatusEvent:
				{
				TCommandStatusEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
									
			case EHardwareErrorEvent:					
				{
				THardwareErrorEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EFlushOccurredEvent:					
				{
				TFlushOccurredEvent event(aEventData);
				aDataEventObserver.MhdeoEventNotification(event);				
				}
				break;

			case ERoleChangeEvent:					
				{
				TRoleChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ENumberOfCompletedPacketsEvent:		
				{
				TNumberOfCompletedPacketsEvent event(aEventData);
				aDataEventObserver.MhdeoEventNotification(event);				
				}
				break;

			case EModeChangeEvent:					
				{
				TModeChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EReturnLinkKeysEvent:				
				{
				TReturnLinkKeysEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EPINCodeRequestEvent:
				{
				TPINCodeRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
				
			case ELinkKeyRequestEvent:
				{
				TLinkKeyRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;
				
			case ELinkKeyNotificationEvent:			
				{
				TLinkKeyNotificationEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ELoopbackCommandEvent:				
				{
				TLoopbackCommandEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EDataBufferOverflowEvent:			
				{
				TDataBufferOverflowEvent event(aEventData);
				aDataEventObserver.MhdeoEventNotification(event);				
				}
				break;

			case EMaxSlotsChangeEvent:				
				{
				TMaxSlotsChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EReadClockOffsetEvent:				
				{
				TReadClockOffsetEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EConnectionPacketTypeChangedEvent:	
				{
				TConnectionPacketTypeChangedEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EQOSViolationEvent:					
				{
				TQOSViolationEvent event(aEventData);
				aDataEventObserver.MhdeoEventNotification(event);				
				}
				break;

			case EPageScanRepetitionModeChangeEvent:	
				{
				TPageScanRepetitionModeChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EFlowSpecificationCompleteEvent:
				{
				TFlowSpecificationCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EInquiryResultwithRSSIEvent:
				{
				TInquiryResultwithRSSIEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EReadRemoteExtendedFeaturesCompleteEvent: 
				{
				TReadRemoteExtendedFeaturesCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ESynchronousConnectionCompleteEvent:	
				{
				TSynchronousConnectionCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ESynchronousConnectionChangedEvent:
				{
				TSynchronousConnectionChangedEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;
				
			case ESniffSubratingEvent:
				{
				TSniffSubratingEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EExtendedInquiryResultEvent:	
				{
				TExtendedInquiryResultEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EEncryptionKeyRefreshCompleteEvent:
				{
				TEncryptionKeyRefreshCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EIOCapabilityRequestEvent:
				{
				TIOCapabilityRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EIOCapabilityResponseEvent:
				{
				TIOCapabilityResponseEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EUserConfirmationRequestEvent:	
				{
				TUserConfirmationRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EUserPasskeyRequestEvent:	
				{
				TUserPasskeyRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ERemoteOOBDataRequestEvent:	
				{
				TRemoteOOBDataRequestEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ESimplePairingCompleteEvent:	
				{
				TSimplePairingCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;
				
			case ELinkSupervisionTimeoutChangedEvent:
				{
				TLinkSupervisionTimeoutChangedEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EEnhancedFlushCompleteEvent:
				{
				TEnhancedFlushCompleteEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EUserPasskeyNotificationEvent:	
				{
				TUserPasskeyNotificationEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case EKeypressNotificationEvent:	
				{
				TKeypressNotificationEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

			case ERemoteHostSupportedFeaturesNotificationEvent:	
				{
				TRemoteHostSupportedFeaturesNotificationEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);				
				}
				break;

		// Deprecated (in the Bluetooth specification) events
			case EPageScanModeChangeEvent:
				{
				TPageScanModeChangeEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

		// Special case events
			case EVendorDebugEvent:
				{
				TVendorDebugEvent event(aEventData);
				aEventObserver.MhceoEventNotification(event);
				}
				break;

			case EBTLogoTestingEvent:
				break;

		// Unknown event
			default:
				rerr = KErrUnknown;
				break;			
			};
		}
	else
		{
		rerr = KErrCorrupt;
		}
	return rerr;		
	}

EXPORT_C TUint8 THCIEventBase::AsTUint8(TInt aOffset) const
	{
	return iEventData[aOffset];
	}
	
EXPORT_C TUint8 THCIEventBase::AsTUint8(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsTUint8(aOffset + (aIndex * aArrayBlockSize));	
	}

EXPORT_C TUint16 THCIEventBase::AsTUint16(TInt aOffset) const
	{
	return TUint16((iEventData[aOffset + 1] << 8) + iEventData[aOffset]);
	}
	
EXPORT_C TUint16 THCIEventBase::AsTUint16(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsTUint16(aOffset + (aIndex * aArrayBlockSize));	
	}

EXPORT_C TUint32 THCIEventBase::AsTUint32(TInt aOffset, TUint8 aSize) const
	{
	TUint32 value = 0;

	switch (aSize)
		{
	case 4:
		value = iEventData[aOffset + 3] << 24; // Fallthrough
	case 3:
		value += iEventData[aOffset + 2] << 16; 
		value += iEventData[aOffset + 1] << 8;
		value += iEventData[aOffset];
		break;
	default:
		PANIC(KSymbianCommandsEventsPanicCat, EParameterOutOfBounds);
		break;
		}

	return value;
	}

EXPORT_C TUint32 THCIEventBase::AsTUint32(TInt aOffset, TInt aArrayBlockSize, TInt aIndex, TUint8 aSize) const
	{
	return AsTUint32(aOffset + (aIndex * aArrayBlockSize), aSize);	
	}

EXPORT_C TUint64 THCIEventBase::AsTUint64(TInt aOffset) const
	{
	TUint tmp = 0;
	tmp = AsTUint32(aOffset, 4);
	TUint64 value = tmp;
	tmp = AsTUint32(aOffset + 4, 4);
	value |= static_cast<TUint64>(tmp) << 32;
	return value;
	}

EXPORT_C TUint64 THCIEventBase::AsTUint64(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsTUint64(aOffset + (aIndex * aArrayBlockSize));	
	}

EXPORT_C TPtrC8 THCIEventBase::AsString(TInt aOffset) const
	{
	return TPtrC8(iEventData.Right(iEventData.Length() - aOffset));
	}
	
EXPORT_C TPtrC8 THCIEventBase::AsNullTerminatedString(TInt aOffset) const
	{
	TPtrC8 fullPtr(iEventData.Right(iEventData.Length() - aOffset));
	TPtrC8 retPtr(fullPtr.Ptr());
	__ASSERT_ALWAYS(retPtr.Length() <= fullPtr.Length(), PANIC(KSymbianCommandsEventsPanicCat, ENullTerminatedStringWasNotNullTerminated));
	return retPtr;
	}

EXPORT_C TPtrC8 THCIEventBase::AsBluetoothDeviceName(TInt aOffset) const
	{
	TPtrC8 fullPtr(iEventData.Right(iEventData.Length() - aOffset));
	TInt len = 0;
	const TUint8* pS = fullPtr.Ptr();
	while(*pS && len < KHCILocalDeviceNameMaxLength)
		{
		pS++;
		len++;
		}
	TPtrC8 retPtr(fullPtr.Left(len));
	__ASSERT_ALWAYS(retPtr.Length() <= fullPtr.Length(), PANIC(KSymbianCommandsEventsPanicCat, ENullTerminatedStringWasNotNullTerminated));
	return retPtr;
	}

EXPORT_C TBTDevAddr THCIEventBase::AsDevAddr(TInt aOffset) const
	{
	TBTDevAddr addr;
	for (TInt i=0;i<KBTMaxDevAddrSize;i++)
		{
		addr[KBTMaxDevAddrSize-1-i] = iEventData[aOffset + i];	// reading bdaddr from iEventBufPtr
		}

	return addr;
	}
	
EXPORT_C TBTDevAddr THCIEventBase::AsDevAddr(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsDevAddr(aOffset + (aIndex * aArrayBlockSize));	
	}

EXPORT_C TBluetoothSimplePairingHash THCIEventBase::AsSimplePairingHash(TInt aOffset) const
	{
	TBluetoothSimplePairingHash retHash;
	retHash.SetMax();
	for (TInt i=0;i<KBluetoothSimplePairingHashSize;i++)
		{
		retHash[KBluetoothSimplePairingHashSize-1-i] = iEventData[aOffset + i];
		}

	return retHash;
	}

EXPORT_C TBluetoothSimplePairingRandomizer THCIEventBase::AsSimplePairingRandomizer(TInt aOffset) const
	{
	TBluetoothSimplePairingRandomizer retRandomizer;
	retRandomizer.SetMax();
	for (TInt i=0;i<KBluetoothSimplePairingRandomizerSize;i++)
		{
		retRandomizer[KBluetoothSimplePairingRandomizerSize-1-i] = iEventData[aOffset + i];
		}

	return retRandomizer;
	}

EXPORT_C TBTLinkKey THCIEventBase::AsLinkKey(TInt aOffset) const
	{
	TBTLinkKey retKey;
	TPtr8 key(retKey().iLinkKey, KHCILinkKeySize, KHCILinkKeySize);
	for (TUint i=0;i<KHCILinkKeySize;i++)
		{
		key[KHCILinkKeySize-1-i] = iEventData[aOffset + i];
		}
		
	return retKey;
	}
	
EXPORT_C TBTLinkKey THCIEventBase::AsLinkKey(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsLinkKey(aOffset + (aIndex * aArrayBlockSize));	
	}

EXPORT_C THCIConnectionHandle THCIEventBase::AsConnectionHandle(TInt aOffset) const
	{
	return THCIConnectionHandle(static_cast<THCIConnHandle>(AsTUint16(aOffset)));
	}

EXPORT_C THCIConnectionHandle THCIEventBase::AsConnectionHandle(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsConnectionHandle(aOffset + (aIndex * aArrayBlockSize));
	}

EXPORT_C TInt8 THCIEventBase::AsTInt8(TInt aOffset) const
	{
	return iEventData[aOffset];
	}
	
EXPORT_C TInt8 THCIEventBase::AsTInt8(TInt aOffset, TInt aArrayBlockSize, TInt aIndex) const
	{
	return AsTInt8(aOffset + (aIndex * aArrayBlockSize));	
	}
	
void THCIEventBase::PutEventCode(const THCIEventCode aValue, TDes8& aEventData)
	{
	TUint8 eventCode = TUint8(aValue); // the event code is a one octet quantity.
	aEventData.Append(&eventCode, 1);
	}

void THCIEventBase::PutTUint8(TUint8 aValue, TDes8& aEventData)
	{
	aEventData.Append(&aValue, 1);
	}

void THCIEventBase::PutTInt8(TInt8 aValue, TDes8& aEventData)
	{
	aEventData.Append(*reinterpret_cast<TUint8*>(&aValue));
	}

void THCIEventBase::PutTUint16(TUint16 aValue, TDes8& aEventData)
	{
	TUint8 val[2] = {aValue & 0xff, aValue >> 8};
	aEventData.Append(val, 2);
	}

void THCIEventBase::PutTUint32(TUint32 aValue, TUint8 aSize, TDes8& aEventData)
	{
	TUint8 val[4];
	switch (aSize)
		{
		// deliberate fallthroughs...
	case 4:
		val[3] = TUint8(aValue>>24);
	case 3:
		val[2] = TUint8(aValue>>16 & 0xff);
	case 2:
		val[1] = TUint8(aValue>>8 & 0xff);
	case 1:
		val[0] = TUint8(aValue & 0xff);
		break;
	default:
		PANIC(KSymbianCommandsEventsPanicCat, EParameterOutOfBounds);
		}
	aEventData.Append(val, aSize);
	}
	
void THCIEventBase::PutTUint64(TUint64 aValue, TDes8& aEventData)
	{
	const TUint32 K32BitMask = 0xFFFFFFFF;
	PutTUint32(aValue & K32BitMask, 4, aEventData);
	PutTUint32((aValue >> 32) & K32BitMask, 4, aEventData);
	}
	
void THCIEventBase::PutString(const TDesC8& aString, TDes8& aEventData)
	{
	aEventData.Append(aString);
	}

void THCIEventBase::PutPaddedString(const TDesC8& aString, TInt aRequiredLength, TDes8& aEventData)
	{
	__ASSERT_DEBUG(aRequiredLength >= aString.Length(), PANIC(KSymbianCommandsEventsPanicCat, EMoreEventDataThanRequired));
	PutString(aString, aEventData);
	const TChar padByte = '\0';
	aEventData.AppendFill(padByte, aRequiredLength - aString.Length());
	}
	
void THCIEventBase::PutDevAddr(const TBTDevAddr& aBdaddr, TDes8& aEventData)
	{
	for (TUint i=0;i<KBTMaxDevAddrSize;i++)
		{
		TUint8 val(aBdaddr[KBTMaxDevAddrSize-1-i]);
		aEventData.Append(&val,1);
		}
	}

void THCIEventBase::PutSimplePairingHash(const TBluetoothSimplePairingHash& aHash, TDes8& aEventData)
	{
	for (TUint i=0;i<KBluetoothSimplePairingHashSize;i++)
		{
		TUint8 val(aHash[KBluetoothSimplePairingHashSize-1-i]);
		aEventData.Append(&val,1);
		}
	}

void THCIEventBase::PutSimplePairingRandomizer(const TBluetoothSimplePairingRandomizer& aRandomizer, TDes8& aEventData)
	{
	for (TUint i=0;i<KBluetoothSimplePairingRandomizerSize;i++)
		{
		TUint8 val(aRandomizer[KBluetoothSimplePairingRandomizerSize-1-i]);
		aEventData.Append(&val,1);
		}
	}

void THCIEventBase::PutLinkKey(const TBTLinkKey aValue, TDes8& aEventData)
	{
	aEventData.Append(aValue().iLinkKey, KHCILinkKeySize);
	}

void THCIEventBase::PutConnectionHandle(THCIConnectionHandle aConnectionHandle, TDes8& aEventData)
	{
	PutTUint16(aConnectionHandle.ConnHandle(), aEventData);
	}

