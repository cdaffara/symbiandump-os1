// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Holds the literals and constants that are used in the Simulator TSY.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __SIMCONSTANTS_H__
#define __SIMCONSTANTS_H__

#include <etel.h>
#include "SimTsy.h"
#include "etelmm.h"

// Constants for the Configuration files
_LIT(KConfigFileDir,"simtsy");				// < The output directory for the SIM TSY.
_LIT8(KSectionNameFormat,"test%d");			// < Section name format within configurations file.
const TInt KDefaultTestNumber=0;			// < The default test number.

const TUint KStdDelimiter=',';				// < Standard Delimiter for config file (a comma)



_LIT8(KGsmPhoneBook, "T2");
_LIT8(KUSimPhoneBook, "T3");

_LIT8(KICCLockedAtStart, "ICCLockedAtStart");
_LIT8(KICCStatus, "ICCStatus");
const TBool KDefaultICCLockedAtStart=0;
const TInt KDefaultPauseDuration= 2;			//< Default Pause
_LIT8(KTriggerEventIPC, "TriggerEventIPC");		//<
_LIT8(KSecurityCodes, "SecurityCodes");
_LIT8(KSecurityCodeAttempts, "SecurityCodeAttempts");
_LIT8(KICCSetting, "ICCSetting");

_LIT8(KDiallingPauseDuration,"DiallingPauseDuration");				// < Configuration file tag for dialling pause.
_LIT8(KConnectingPauseDuration,"ConnectingPauseDuration");			// < Configuration file tag for connecting pause.
_LIT8(KDisconnectingPauseDuration,"DisconnectingPauseDuration");	// < Configuration file tag for disconnecting pause.
_LIT8(KAnswerIncomingPauseDuration,"AnswerIncomingPauseDuration");	// < Configuration file tag for answer incoming pause.
_LIT8(KNotifyIncomingCallPauseDuration, "NotifyIncomingCallPauseDuration"); // < Configuration file tag for notify incoming call pause.
_LIT8(KRemoteHangupPauseDuration,"RemoteHangupPauseDuration");	// < Configuration file tag for remote hangup pause.

_LIT8(KNotifyRemotePartyInfo, "RemotePartyInfo");

_LIT8(KDataCallCaps,"DataCallCaps");
_LIT8(KDynamicHSCSDInfo,"DataHscsdInfo");
_LIT8(KDataRLPparams,"DataRlpParams");

const TInt KDefaultDiallingPauseDuration=1;			// < Default dialling pause
const TInt KDefaultConnectingPauseDuration=1;		// < Default connecting pause
const TInt KDefaultDisconnectingPauseDuration=1;	// < Default disconnecting pause
const TInt KDefaultAnswerIncomingPauseDuration=1;	// < Default answer incoming call pause
const TInt KDefaultNotifyIncomingCallPauseDuration=0;	// < Default notify incoming call pause
const TInt KDefaultRemoteHangupPauseDuration=1;		// < Default answer remote hangup pause

_LIT8(KSmsRx,"SmsRx");							// < Configuration file tag for SMS reception
_LIT8(KSmsTx,"SmsTx");							// < Configuration file tag for SMS transmission
_LIT8(KSmsRxPeriod,"SmsRxPeriod");				// < Configuration file tag for SMS reception pause
_LIT8(KSmsAckNackPause,"SmsAckNackPause");		// < Configuration file tag for SMS ack/nack pause
_LIT8(KSmsResumePause,"SmsResumePause");		// < Configuration file tag for SMS resume pause
_LIT8(KSmsStartRxDelay,"SmsStartRxDelay");		// < Configuration file tag for SMS start reception flag

_LIT8(KSmsStartRxDelay2,"SmsStartRxDelay2");	// < Configuration file tag for SMS start reception flag

_LIT8(KSmsTxPause,"SmsTxPause");				// < Configuration file tag for SMS transmition pause
_LIT8(KSmsControlCaps,"SmsControlCaps");		// < Configuration file tag for SMS control capabilities

const TInt KDefaultSmsRxPeriod=1;				// < Default period between simulating MT SMS
const TInt KDefaultSmsAckNackPause=1;			// < Default pause before completing Ack/Nack
const TInt KDefaultSmsResumePause=1;			// < Default pause before completing Resume
const TInt KDefaultSmsTxPause=1;				// < Default pause before completing Resume

const RMobileSmsMessaging::TMobileSmsControlCaps KDefaultSmsControlCaps=RMobileSmsMessaging::TMobileSmsControlCaps(0x1c7);			// < Default control capabilities


_LIT8(KSmsModeCaps,"SmsModeCaps");				// < Configuration file tag for SMS mode capabilities
const RMobileSmsMessaging::TMobileSmsModeCaps KDefaultSmsModeCaps=RMobileSmsMessaging::KCapsGsmSms; // < Default mode is GSM

_LIT8(KSmsStore,"SmsStore");					// < Configuration file tag for SMS Store definition
_LIT8(KSmsStoreEntry,"SmsStoreEntry");				// < Configuration file tag for SMS Store entry
_LIT8(KSmsPhoneStoreCaps,"SmsPhoneStoreCaps");		// < Configuration file tag for Phone store caps for SmsStore 

const TInt KDefaultSmsStoreIndividualReqPause=1;	// < Default pause before completing an "individual" phonebook request
const TInt KDefaultSmsStoreBatchReqPause=1;			// < Default pause before completing a "batch" phonebook request
const TUint KDefaultSmsPhoneStoreCaps=0xFC000000;	// < Default Phone store caps for SmsStore 
const TUint KDefaultSmsOnlySmsCaps=0x1f;			// < Default SmsStore level  caps for sms store. SmsStoreCaps=SmsPhoneStoreCaps+SmsOnlyStoreCaps


_LIT8(KSmsStoreIndividualReqPause,"SmsStoreIndividualReqPause");	// < Configuration file tag for pause of an individual SMS Store operation
_LIT8(KSmsStoreBatchReqPause,"SmsStoreBatchReqPause");		// < Configuration file tag for pause of a batch SMS Store operation
_LIT8(KPhBkPhoneStoreCaps,"PhBkPhoneStoreCaps");		// < Configuration file tag for Phone store caps for PhBkStore 
_LIT8(KPhBkPhoneUSimStoreCaps,"PhBkPhoneUSimStoreCaps");

const TInt KDefaultPhBkStoreIndividualReqPause=1;			// < Default pause before completing an "individual" phonebook request
const TInt KDefaultPhBkStoreBatchReqPause=1;				// < Default pause before completing a "batch" phonebook request
const TUint KDefaultPhBkPhoneStoreCaps=0xFC000000;	// < Default Phone store caps for PhBk 

_LIT8(KPhBkStore,"PhBkStore");				// < Configuration file tag for phonebook store definition
_LIT8(KPhBkUSimStore,"PhBkUSimStore");				// < Configuration file tag for USIM phonebook store definition

_LIT8(KPhBkStoreEntry,"PhBkStoreEntry");	// < Configuration file tag for phonebook store entry
_LIT8(KPhBkUSimStoreEntry,"PhBkUSimStoreEntry");	// < Configuration file tag for USIM phonebook store entry

_LIT8(KPhBkError,"PhBkError");				// < Configuration file tag for phonebook simulated error
_LIT8(KOOBPhBkWrite,"OOBPhBkWrite");		// < Configuration file tag for "out of band" phonebook write
_LIT8(KOOBPhBkDelete,"OOBPhBkDelete");		// < Configuration file tag for "out of band" phonebook delete
_LIT8(KOOBPhBkUSimWrite,"OOBPhBkUSimWrite");		// < Configuration file tag for "out of band" USIM phonebook write
_LIT8(KOOBPhBkUSimDelete,"OOBPhBkUSimDelete");
_LIT8(KPhBkStoreIndividualReqPause,"PhBkStoreIndividualReqPause");	// < Configuration tag for pause of an individual phonebook operation
_LIT8(KPhBkStoreBatchReqPause,"PhBkStoreBatchReqPause");			// < Configuration tag for pause of a batch phonebook operation

const TInt KDefaultONStoreIndividualReqPause=1;			// < Default pause before completing an "individual" Own Number store request
const TUint KDefaultONPhoneStoreCaps=0xFC000000;	// < Default Own Number store caps 
_LIT8(KONStore,"ONStore");				// < Configuration file tag for Own Number store definition
_LIT8(KONStoreEntry,"ONStoreEntry");	// < Configuration file tag for Own Number store entry
_LIT8(KONPhoneStoreCaps,"ONPhoneStoreCaps");		// < Configuration file tag for Own Number store caps 
_LIT8(KONStoreIndividualReqPause,"ONStoreIndividualReqPause");	// < Configuration tag for pause of an individual Own Number store operation

_LIT8(KSmsParam,"SmsParam");
_LIT8(KSmsParamEntry,"SmsParamEntry");
_LIT8(KSmspIndividualReqPause,"SmspIndividualReqPause");
_LIT8(KSmspBatchReqPause,"SmspBatchReqPause");
const TInt KDefaultSmspIndividualReqPause=1;	// < Default pause before completing an "individual" phonebook request
const TInt KDefaultSmspBatchReqPause=1;			// < Default pause before completing a "batch" phonebook request

_LIT8(KSSLevel,"SignalStrengthLevel");		// < Configuration tag for Signal Strength level information
_LIT8(KSSNotifyCap,"NotifySignalStrengthChangeCap"); // < Configuration tag for Signal Strength notification capability

_LIT8(KIndicators,"Indicators");		// < Configuration tag for Signal Strength level information
_LIT8(KIndicatorCheckPeriod,"IndicatorCheckPeriod");	// < Configuration tag for Indicator check period
_LIT8(KBatteryCharger,"BatteryCharger");		// < Configuration tag for Battery Charger information

_LIT8(KSmspMaxNumSlots,"SmsParamMaxNumSlots");		// < configuration file tag for number of entries in a smsp list
const TInt KDefaultSmspMaxNumSlots=1;				// < Default Number of entries in a smsp list

_LIT8(KRegStatus,"RegStatus");				// < Configuration tag for Registration Status information
_LIT8(KNetworkInfo,"NetworkInfo");			// < Configuration tag for Network Information information
_LIT8(KNitzInfo,"NitzInfo");				// < Configuration tag for NITZ information
_LIT8(KCellInfo,"CellInfo");				// < Configuration tag for Cell information
_LIT8(KLocationArea,"LocationArea");		// < Configuration tag for location area information


_LIT8(KRadioMode,"RadioMode");				// < Configuration tag for NITZ information
const TUint KFirstLACDefault=0;			// <  Default for First Location area code for defining the entire range of LACs for the registered PLMN.

const TUint KLastLACDefault = 3;		// <  Default for  Last Location area code defining the entire range of LACs for the registered PLMN.

_LIT8(KServiceProviderName,"ServiceProviderName");				// < Configuration tag for Service Provider
const RMobilePhone::TDisplayRequirements KDisplayReqDefault =  RMobilePhone::KDisplayPLMNNotRequired;	//Default display requirements for service provider
_LIT8(KSPNameDefault, "Vodafone");
_LIT8(KPLMNFieldDefault, "\x50\x03\x21\x44\x21");	// < Default PLMN field TLV buffer.  This corresponds to tag=80, len=3,val=(MCC=123, MNC=123)

_LIT8(KSubscriberId,"SubscriberId");			// < Configuration tag for Subscriber ID
_LIT8(KPhoneId,"PhoneId");			// < Configuration tag for PhoneId
_LIT8(KPhoneManufacturerDefault, "Symbian");	// < Default Manufacturer 
_LIT8(KPhoneModelDefault, "888");	// < Default Model 
_LIT8(KPhoneRevesionDefault, "0.1");	// < Default Revision 
_LIT8(KPhoneSerialNumberDefault, "987654321");	// < Default Serail Number 

_LIT8(KNetworkMode,"NetworkMode");				// < Configuration tag for Network Mode
const TInt KNetworkModeDefault=(TInt)RMobilePhone::ENetworkModeGsm;
												// < Default Network Mode (used if no network mode is present)

_LIT8(KCommSetup,"CommSetup");					// < Configuration tag for communications port set-up.

_LIT8(KBearerService,"BearerService");			// < Configuration tag for Bearer Service information.
const TUint32 KDefaultBearerCaps=RCall::KBearerCapsProtocolNone;	// < Default Bearer Capability information, used if no configuration file tag is present.
const RCall::TCallBearerSpeed KDefaultBearerSpeed=RCall::EBearerData9600;	// < Default Bearer Speed information, used if no configuration file tag is present.

// default notification type is 'Timer'
_LIT8(KNotificationTypeTimer, "Timer");
_LIT8(KNotificationTypePublishSubscribe, "PublishSubscribe");

_LIT8(KPacketRegStatus,"PacketRegStatus");
_LIT8(KNetworkRegStatusType,"NetworkRegStatusType");
_LIT8(KNetworkRegStatus,"NetworkRegStatus");
_LIT8(KDynamicCaps,"DynamicCaps");

_LIT8(KQosProfileReqGPRS,"QosProfileReqGPRS");
_LIT8(KQosProfileNegGPRS,"QosProfileNegGPRS");
//_LIT8(KQosProfileReqCDMA,"QosProfileReqCDMA");
//_LIT8(KQosProfileNegCDMA,"QosProfileNegCDMA");
_LIT8(KQosProfileCapsGPRS,"QosProfileCapsGPRS");
//_LIT8(KQosProfileCapsCDMA,"QosProfileCapsCDMA");

_LIT8(KQosProfileReqR99, "QosProfileReqR99");
_LIT8(KQosProfileNegR99, "QosProfileNegR99");
_LIT8(KQosProfileCapsR99, "QosProfileCapsR99");

_LIT8(KQosProfileReqR5, "QosProfileReqR5");
_LIT8(KQosProfileNegR5, "QosProfileNegR5");
_LIT8(KQosProfileCapsR5, "QosProfileCapsR5");

_LIT8(KQosNetworkNotification, "QosNetworkNotification");
_LIT8(KQosR5NetworkNotification, "QosR5NetworkNotification");
_LIT8(KQosNetworkNotificationType, "QosNetworkNotificationType");
_LIT8(KSetProfileFail, "SetProfileFail");

_LIT8(KSetConfigFail, "SetConfigFail");
_LIT8(KSetConfigDelay, "SetConfigDelay");
_LIT8(KDeleteTftFail, "DeleteTftFail");
_LIT8(KCreateTftConfig, "CreateTftConfig");
_LIT8(KAddPacketFilter, "AddPacketFilter");
_LIT8(KSetQoSDelay, "SetQoSDelay");


/****  */

_LIT8(KPacketReleaseMode, "ReleaseMode");
_LIT8(KContextConfigRel99,"R99ContextConfig");

_LIT8(KSetContextConfigGPRS,"SetContextConfigGPRS");
_LIT8(KDefaultContextparamGPRS,"DefaultContextparamGPRS");
_LIT8(KDefaultGPRSAdditionalParamItem, "DefaultGPRSAdditionalParamItem");
_LIT8(KAdditionalParamItemGPRS, "AdditionalParamItemGPRS");

//_LIT8(KSetContextConfigCDMA,"SetContextConfigCDMA");
//_LIT8(KDefaultContextparamCDMA,"DefaultContextparamCDMA");

_LIT8(KSetContextConfigRel99,"SetContextConfigR99");
_LIT8(KDefaultContextparamRel99,"DefaultContextparamRel99");
_LIT8(KDefaultAdditionalParamItemRel99,"DefaultAdditionalParamItemRel99");
_LIT8(KAdditionalParamItemRel99, "AdditionalParamItemRel99");

_LIT8(KSetContextConfigRel5,"SetContextConfigR5");
_LIT8(KAdditionalParamItem, "AdditionalParamItem");
_LIT8(KDefaultContextParamRel5,"DefaultContextParamRel5");
_LIT8(KDefaultAdditionalParamItem, "DefaultAdditionalParamItem");


_LIT8(KDelayIncomingContext, "DelayIncomingContext");
_LIT8(KNoTypeIndex, "NoTypeIndex");


_LIT8(KPacketFilterInfo, "TFTfilter");
_LIT8(KNotifyContextStatusChange, "NotifyContextStatusChange");
_LIT8(KNotifyContextStatusChangeType, "NotifyContextStatusChangeType");
_LIT8(KNotifyContextConfigChange, "NotifyContextConfigChange");


_LIT8(KAuthorizationInfo, "IMSAuthorizationInfo");
_LIT8(KAuthenticationInfo, "IMSAuthenticationInfo");

// Services Control table entry's
_LIT8(KUSIMServiceTableSupportedEntrys, "USIMServiceTableSupportedEntrys");
_LIT8(USIMServiceTableEnabledEntrys, "USIMServiceTableEnabledEntrys");
_LIT8(KSIMServiceTable, "SIMServiceTable");
//_LIT8(KCDMAServiceTable, "CDMAServiceTable");

// EF - ACL (Access Control List)
_LIT8(KAccessPointNameList, "APNControlList");

// RMobilePhone::SmartCardAuthenticate() response info
_LIT8(KScAuthInfo, "SmartCardAuthInfo");

// Constants for the Phone
const TInt KNumberOfPhones=1;				//< Only one phone is supported by this TSY.
_LIT8(KPhoneStatus, "PhoneStatus");
_LIT8(KPhoneInitialiseDelay, "PhoneInitialiseDelay");

// Constants for the line
const TInt KNumberOfLines=2;				//< Two lines (voice and data) are supported by this TSY.
const TInt KVoiceLineIndex=0;				//< The line index number for the voice line, used for the phone-based GetInfo request.
const TInt KDataLineIndex=1;				//< The line index number for the data line, used for the phone-based GetInfo request.

//Constants for Calls
_LIT(KVoiceCallName,"SimCall");				//< Voice call name
_LIT(KDataCallName,"DataCall");				//< Data call name

//Constants for Sms Messaging and Sms Store
_LIT(KSimSmsMessagingName,"SimSmsMessaging");	//< The name of the Sms messaging supported by this TSY.
_LIT(KSimSmsStoreName,"SimSmsStore");			//< The name of the Sms store supported by this TSY.

//Constants for PhoneBook
_LIT(KPhoneBookName,"SimPhoneBook");		//< The name of the phone book supported by this TSY.

//Constants for packet
_LIT(KSimPacketName,"SimPacket");				//< The name of the packet supported by this TSY.

//Constants for packet
_LIT(KSimPacketServiceName,"PACKET_SERVICE");				//< The name of the packet supported by this TSY.
_LIT(KSimPacketPrimaryContextName,"PRIMARY");
_LIT(KSimPacketSecondaryContextName,"SECONDARY");

_LIT(KSimPacketQosName,"PACKET_QOS");
_LIT8(KAttachPauseDuration,"AttachPauseDuration");
_LIT8(KDetachPauseDuration,"DetachPauseDuration");

_LIT8(KActivatePauseDuration,"ActivatePauseDuration");
_LIT8(KDeactivatePauseDuration,"DeactivatePauseDuration");
_LIT8(KDeletePauseDuration,"DeletePauseDuration");

// constants for logging names
_LIT(KSimLogDir,"simtsy");				// < Simulator TSY log file directory .
_LIT(KSimLogFile,"simtsy.txt");			// < Simulator TSY log file.

//Constants for CSIMSAT
_LIT(KSatName,"TOOLKIT");		//< Name of the sat to be opened by CSimPhone
_LIT8(KSatRefresh,"SatRefresh");		// < Configuration tag for Sat
_LIT8(KMMRetrieve,"MMRetrieve");	//<  Configuration tag for EtelSat MMRetrieve Proactive command
_LIT8(KMMSubmit,"MMSubmit"); 		//<  Configuration tag for EtelSat MMSubmit Proactive command
_LIT8(KMMDisplay,"MMDisplay");		//<  Configuration tag for EtelSat MMDisplay Proactive command
_LIT8(KSetFrames,"SetFrames");	//<  Configuration tag for EtelSat SetFrames Proactive command
_LIT8(KGetFramesStatus,"GetFramesSts"); //< Configuration tag for EtelSat GetFramesSts Proactive command
_LIT8(KMmsNotificationDownload,"MmsNotificationDownload");	//< Configuration tag for EtelSat MMS Notification Download command
_LIT8(KUssdDataDownload,"UssdDataDownload");	//< Configuration tag for EtelSat USSD Data Download command
_LIT8(KLocalInfo,"ProvideLocalInfo"); //< Configuration tag for EtelSat PROVIDE LOCAL INFORMATION command
_LIT8(KOpenChnl,"OpenChannel"); //< Configuration tag for EtelSat OPEN CHANNEL PROACTIVE COMMAND
_LIT8(KMiscCmd,"MiscCmd");	//< Configuration tag for EtelSat PLAY TONE PROACTIVE COMMAND
_LIT8(KEventDnld,"EventDnld"); //< Configuration tag for EtelSat EVENT DOWNLOAD COMMAND
_LIT8(KSendUssd,"SendUssd");	//<Configuration tag for EtelSat SEND USSD command
_LIT8(KSendSs,"SendSs"); //<Configuration tag for EtelSat SEND SS command
_LIT8(KRefresh2,"Refresh"); //<Configuration tag for EtelSat REFRESH command
_LIT8(KElemFiles,"ElemFiles"); //<Configuration tag for EtelSat REFRESH command
_LIT8(KLaunchBrowser,"LnchBrwsr"); //<Configuration tag for EtelSat LAUNCH BROWSER command

//Constants for callforwarding
_LIT8(KCFList,"CFList");
_LIT8(KPhoneNumber, "PhoneNumber");
_LIT8(KCallForwardingInfo, "CallForwardingInfo");
_LIT8(KIdentityServiceStatus, "IdentityServiceStatus");
_LIT8(KEmptyString, "");

//Constant for CCallBarring
_LIT8(KCallBarringInfo, "CallBarringInfo");
_LIT8(KCBList,"CBList");
_LIT8(KCBPassword, "CBPassword");
_LIT8(KCBDefaultPassword, "0000");

//Constant for CCallWaiting
_LIT8(KCWList,"CWList");

//Constants for USIM Application support
_LIT8(KUSIMAppInfo, "USIMAppInfo");
_LIT8(KUSIMAppSwitching, "USIMAppSwitching");

//Constants for SmartCard Application support
_LIT8(KMaxActiveSmartCardApps, "MaxActiveSmartCardApps");
_LIT8(KActiveUSIMApp, "ActiveUSIMApp");
_LIT8(KSmartCardAppInfo, "SmartCardAppInfo");
_LIT8(KSmartCardAppEvent, "SmartCardAppEvent");
_LIT8(KSmartCardFile, "SmartCardFile");

#define CB_ACTIVATE_ALL_OUTGOING  _L("**33*")  //Activate barr all outgoing calls 
#define CB_DEACTIVATE_ALL_OUTGOING  _L("#33*")  //DeActivate barr all outgoing calls  
#define CB_CHECK_STATUS  _L("*#33#")  //Check status of barr all outgoing calls

//Constants for Flash With Info Message (CDMA Call)
_LIT8(KFlashCallRx,"FlashCallRx");

//
// Definitions for CDMA SimTsy Features
//

/*//Constants for Incoming Info Record Notifications
_LIT8(KInfoRecordRx,"InfoRecordRx");

//Constants for Preferred Languages
_LIT8(KPrefLangList,"PrefLangList");
_LIT8(KPrefLangTimers,"PrefLangTimers");
_LIT8(KPrefLangListChange,"PrefLangListChange");

//Constants for Location Privacy
_LIT8(KLocPriSetting,"LocPriSetting");
_LIT8(KLocPriTimers,"LocPriTimers");

//Constants for Tty Mode
_LIT8(KTtyMode,"TtyMode");
_LIT8(KTtyModeTimers,"TtyModeTimers");

//Constants for Ruim
_LIT8(KRuimRevNum,"RuimRevNum");
_LIT8(KRuimTimers,"RuimTimers");*/

//Constants for Roaming Status
_LIT8(KRoamStatTimers,"RoamStatTimers");

/*//Constants for Ota
_LIT8(KOtaTimers,"OtaTimers");
_LIT8(KOtaSetting,"OtaSetting");
_LIT8(KOtaStatus,"OtaStatus");

//Constants for Emergency Mode
_LIT8(KEmergModeTimers,"EmergencyModeTimers");

//Constants for DTMF Mode and Duration
_LIT8(KDtmfMode,"DtmfMode");
_LIT8(KDtmfModeTimers,"DtmfModeTimers");
_LIT8(KDtmfDuration,"DtmfDuration");
_LIT8(KDtmfDurationTimers,"DtmfDurationTimers");*/

_LIT8(KTsyMode,"TsyMode");

//Constants for Authorization Tokens and Flow Identifiers
_LIT8(KAuthorizationToken,"AuthorizationToken");
_LIT8(KFlowIdentifier, "FlowIdentifier");

//Constants for use in config.txt to describe EAP procedures
_LIT8(KScEapProcedures, "EapProcedure");
_LIT8(KScEapIdentity, "EapId");
_LIT8(KScEapPsIdentity, "EapPseudonymId");
_LIT8(KScEapKeyMSK, "EapMasterKey");
_LIT8(KScEapKeyEMSK, "EapExtMasterKey");
_LIT8(KScEapChallenge, "EapChallenge");

//Constants for USIM Release 6 support
_LIT8(KUsimGBA,"GBAAuth");
_LIT8(KUsimMailBoxIdInfo,"MailBoxIdInfo");
_LIT8(KUsimMessageWaitingIdInfo,"MessageWaitingIdInfo");
_LIT8(KUsimWlanDataInfo,"WlanDataInfo");
_LIT8(KUsimWlanSidInfo,"WlanSidInfo");
_LIT8(KUsimPreferredNetworksInfo,"PreferredNetworkInfo");

//Constants for Simulated Version Switching
const TInt KSimTsyDefaultVersion = 9999;
_LIT8(KTsyVersionNumber, "TsyVersionNumber");

const TInt KNumberofConfigs = 11; //< Chosen at random.
const TInt KMaxNumberofConfigsChanges = 11; //< Chosen at random.

//Constants to identify the command number
#define SIMTSY_RET_MM_PCMD_NUMBER		0xB1
#define SIMTSY_SUB_MM_PCMD_NUMBER		0xB2
#define SIMTSY_DISP_MM_PCMD_NUMBER		0xB3
#define SIMTSY_SET_FRMS_PCMD_NUMBER		0xB4
#define SIMTSY_GET_FRMS_STS_PCMD_NUMBER		0xB5
#define SIMTSY_PRV_LCL_INFO_PCMD_NUMBER 	0xB6
#define SIMTSY_OPEN_CHAN_PCMD_NUMBER		0xB7
#define SIMTSY_PLAY_TONE_PCMD_NUMBER		0xB8
#define SIMTSY_SETUP_CALL_PCMD_NUMBER		0xB9
#define SIMTSY_REFRESH_PCMD_NUMBER		0xBA
#define SIMTSY_SEND_USSD_PCMD_NUMBER		0xBB
#define SIMTSY_GET_INKEY_PCMD_NUMBER		0xBC
#define SIMTSY_LNCH_BRWSR_PCMD_NUMBER           0xBD
#define SIMTSY_EVENT_LIST_CMD_NUMBER		0xBE
#define SIMTSY_SENDSS_CMD_NUMBER		0xBF
#define SIMTSY_ELEMFILES_CMD_NUMBER		0xC1
#define SIMTSY_USSD_CMD_NUMBER			0xC3
#define SIMTSY_MMSNOTIFICATIONDOWNLOAD_CMD_NUMBER		0xC4

//Constants for USIM Release 6 support
#define SIMTSY_GBA_NAFLIST_COUNT 	(3)
#define SIMTSY_MBMS_MSKLIST_COUNT 	(3)

// RMobilePhone::GBAAuthentication information parameters
_LIT8(KGBAAuthInfo, "GBAAuthInfo");
_LIT8(KGBAListInfo, "GBAListInfo");
_LIT8(KMBMSInfo, "MBMSInfo");
_LIT8(KMBMSListInfo,"MBMSListInfo");

_LIT8(KConfigScWlanAuthAUTN, "WLAN_AUTN_IN");
_LIT8(KConfigScWlanAuthRAND, "WLAN_RAND_IN");
_LIT8(KConfigScWlanAuthRES, "WLAN_RES_OUT");
_LIT8(KConfigScWlanAuthIK, "WLAN_IK_OUT");
_LIT8(KConfigScWlanAuthCK, "WLAN_CK_OUT");

_LIT8(KConfigScWlan, "WLAN");
_LIT8(KConfigScWlanNotSupported,"NotSupported");
_LIT8(KWlanAid, "WlanAid");

#define SIMTSY_PHONE_WLAN_SIDLIST_COUNT		(3)

#define SIMTSY_PHONE_WLAN_PSEUDONYM			(RMobilePhone::KWlanPseudonym)
#define SIMTSY_PHONE_WLAN_REAUTHID			(RMobilePhone::KWlanReauthenticationId)
#define SIMTSY_PHONE_WLAN_MASTERKEY			(RMobilePhone::KWlanMasterKey)
#define SIMTSY_PHONE_WLAN_COUNTER			(RMobilePhone::KWlanCounter)
#define SIMTSY_PHONE_WLAN_LIST_DATA_FLAGS1	((SIMTSY_PHONE_WLAN_PSEUDONYM | \
											  SIMTSY_PHONE_WLAN_REAUTHID |\
											  SIMTSY_PHONE_WLAN_MASTERKEY |\
											  SIMTSY_PHONE_WLAN_COUNTER ))

#define SIMTSY_PHONE_WLAN_LIST_DATA_FLAGS	((SIMTSY_PHONE_WLAN_PSEUDONYM | \
											  SIMTSY_PHONE_WLAN_REAUTHID))

#define SIMTSY_PHONE_WLAN_VALID_PSEUDONYM	(_L8("SIM_VAL_PNYM__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345690301234567890123456123432"))			
#define SIMTSY_PHONE_WLAN_VALID_REAUTHID	(_L8("SIM_VAL_REAID_ABCD78ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678908712345678901234523435"))		
#define SIMTSY_PHONE_WLAN_VALID_MASTERKEY	(_L8("SIM_VAL_MASTERKEY_EFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678123456123435"))	
#define SIMTSY_PHONE_WLAN_VALID_COUNTER		(_L8("SIM_VAL_COUNTER_CDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678923456123435"))		

#define SIMTSY_PHONE_WLAN_INVALID_PSEUDONYM	(_L8("SIM_INV_PNYM"))			
#define SIMTSY_PHONE_WLAN_INVALID_REAUTHID	(_L8("SIM_INV_REAID"))		
#define SIMTSY_PHONE_WLAN_INVALID_MASTERKEY	(_L8("SIM_INV_MASTERKEY"))
#define SIMTSY_PHONE_WLAN_INVALID_COUNTER	(_L8("SIM_INV_CTR"))		

#define SIMTSY_PHONE_WLAN_ACESS_DENIED	(_L8("ACCESS_DENIED"))

#define SIMTSY_PHONE_WLAN_WSID_ONE			(_L8("Wlan"))
#define SIMTSY_PHONE_WLAN_USERDEFD_ONE		(TBool)(ETrue)

#define SIMTSY_PHONE_WLAN_WSID_TWO			(_L8("Wlan"))
#define SIMTSY_PHONE_WLAN_USERDEFD_TWO		(TBool)(ETrue)

#define SIMTSY_PHONE_WLAN_WSID_THREE			(_L8("Wlan"))
#define SIMTSY_PHONE_WLAN_USERDEFD_THREE	(TBool)(EFalse)

#define SIMTSY_PHONE_WLAN_INVALID_WSID_ONE		(_L8("SIM_INV_SID_1"))
#define SIMTSY_PHONE_WLAN_INVALID_USERDEFD_ONE 	(TBool)(ETrue)

#define SIMTSY_PHONE_WLAN_INVALID_WSID_TWO		(_L8("SIM_INV_SID_2"))
#define SIMTSY_PHONE_WLAN_INVALID_USERDEFD_TWO 	(TBool)(EFalse)

#define SIMTSY_PHONE_WLAN_INVALID_WSID_THREE		(_L8("SIM_INV_SID_3"))
#define SIMTSY_PHONE_WLAN_INVALID_USERDEFD_THREE 	(TBool)(EFalse)

#define SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM			  (RMobilePhone::KNetworkAccessGsm)
#define SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC			  (RMobilePhone::KNetworkAccessGsmCompact)
#define SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN			  (RMobilePhone::KNetworkAccessUtran)
#define SIMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN			  (RMobilePhone::KNetworkAccessWLAN)

#define SIMTSY_PHONE_NTWK_LIST_NETWORK_ID1				  _L("MNC1")
#define SIMTSY_PHONE_NTWK_LIST_NETWORK_ID2				  _L("MNC2")
#define SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID1				  _L("MCC1")
#define SIMTSY_PHONE_NTWK_LIST_COUNTRY_ID2				  _L("MCC2")

#define SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE		  (TBool)(ETrue)
#define SIMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE		  (TBool)(EFalse)

#define SIMTSY_PHONE_NTWK_LIST_ENTRIES					(4)

_LIT8(KMBMSBroadCastPacketServiceParams, "MBMSBroadCastPacketServiceParams");
_LIT8(KMBMSBroadcastMonitorList,"MBMSBroadcastMonitorList");
_LIT8(KMBMSSessionIdList,"MBMSSessionIdList");

#define SIMTSY_PACKET_MBMS_ADD_ENTRIES_COUNT				(2)
#define SIMTSY_PACKET_MBMS_REM_ENTRIES_COUNT				(2)

#define SIMTSY_PACKET_MBMS_ADD_ENTRIES				EAddEntries
#define SIMTSY_PACKET_MBMS_REM_ENTRIES				ERemoveEntries
#define SIMTSY_PACKET_MBMS_REM_ALL_ENTRIES			ERemoveAllEntries

#endif // __SIMCONSTANTS_H__

