// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll
*/


#ifndef __TESTDEF_H
#define __TESTDEF_H

#include "ETELMM.H"


// General Testing name definitions
const TInt DMMTSY_TIMEOUT = 100000;

#define DMMTSY_LINE_VOICE_NAME			_L("Voice")
#define DMMTSY_LINE_DATA_NAME			_L("Data")
#define DMMTSY_LINE_FAX_NAME			_L("Fax")
#define DMMTSY_LINE_VOICE2_NAME			_L("Voice2")

#define DMMTSY_CALL_NAME1				_L("Call1")
#define DMMTSY_CALL_NAME2				_L("Call2")
#define DMMTSY_CALL_NAME3				_L("Call3")

#define DMMTSY_PHONE_NAME				_L("DMobile")
#define DMMTSY_PHONE_NAMEAUTH1		    _L("DMobileAuth1")
#define DMMTSY_PHONE_NAMEAUTH2	    	_L("DMobileAuth2")
#define DMMTSY_PHONE_NAMEAUTH3		    _L("DMobileAuth3")
#define DMMTSY_MODULE_NAME				_L("DMM")

#define DMMTSY_PHONE_TEL_NUMBER			_L("0123 456 7890")
#define DMMTSY_PHONE_TON				RMobilePhone::EInternationalNumber
#define DMMTSY_PHONE_NPI				RMobilePhone::EIsdnNumberPlan

// CoreTsy functionality
#define DMMTSY_CALL_BEARER_CAPS			RCall::KBearerCapsProtocolALT
#define DMMTSY_CALL_BEARER_SPEED		RCall::EBearerData4800

#define DMMTSY_FAX_DATA					_L8("0123456789")

#define DMMTSY_NETWORK_TYPE				(RTelServer::ENetworkTypeMobileDigital)
#define DMMTSY_NUMBER_OF_PHONES			(4)
#define DMMTSY_NUMBER_OF_LINES			(4)

#define DMMTSY_CORE_CALL_STATUS			RCall::EStatusIdle
#define DMMTSY_CALL_DURATION1			(10)
#define DMMTSY_CORE_LINE_CAPS			(RLine::KCapsVoice)
#define DMMTSY_CORE_LINE_STATUS			RCall::EStatusIdle
#define DMMTSY_CORE_LINE_STATUS2		RCall::EStatusRinging
#define DMMTSY_CORE_PHONE_STATUS		RPhone::EDetectedPresent
#define DMMTSY_GET_PROGRESS_SPEED		(11)
#define DMMTSY_MYFAX					_L("MY FAX")

// Testing RMobilePhone

#define DMMTSY_PHONE_MULTIMODE_CAPS			(TUint32)(RMobilePhone::KCapsGsmSupported	|\
											 RMobilePhone::KCapsCdma95Supported	|\
											 RMobilePhone::KCapsAmpsSupported)

#define DMMTSY_PHONE_STORE_COUNT			(2)

#define DMMTSY_PHONE_STORE_INDEX1			(0)
#define DMMTSY_PHONE_STORE_NAME1			KETelIccSmsStore
#define DMMTSY_PHONE_STORE_TYPE1			(RMobilePhoneStore::EShortMessageStore)
#define DMMTSY_PHONE_STORE_USED_ENTRIES1	(6)
#define DMMTSY_PHONE_STORE_TOTAL_ENTRIES1	(100)
#define DMMTSY_PHONE_STORE_CAPS1			(TUint32)(RMobilePhoneStore::KCapsWholeStore |\
											 RMobilePhoneStore::KCapsIndividualEntry)
#define DMMTSY_PHONE_STORE_INDEX2			(1)
#define DMMTSY_PHONE_STORE_NAME2			(KETelMeSmsStore)
#define DMMTSY_PHONE_STORE_TYPE2			(RMobilePhoneStore::EShortMessageStore)
#define DMMTSY_PHONE_STORE_USED_ENTRIES2	(12)
#define DMMTSY_PHONE_STORE_TOTAL_ENTRIES2	(255)
#define DMMTSY_PHONE_STORE_CAPS2			(TUint32)(RMobilePhoneStore::KCapsWholeStore |\
											 RMobilePhoneStore::KCapsIndividualEntry)

#define DMMTSY_PHONE_SIM_ACCESS_CAPS1		(TUint32)(RMobilePhone::KCapsSimAccessSupported)
#define DMMTSY_PHONE_SIM_ACCESS_CAPS2		(0)
#define DMMTSY_PHONE_ICC_ID					_L8("ICCID1")

#define DMMTSY_PHONE_CONFERENCE_CAPS		RMobilePhone::KCapsSimAccessSupported
#define DMMTSY_PHONE_CONTINUE_DTMF_SENDING	ETrue

#define DMMTSY_PHONE_CSP_BYTE1				(0xF0) // CFB, CFU, CFNRy, CFNrc
#define DMMTSY_PHONE_CSP_BYTE2				(0xF1) // all barring services
#define DMMTSY_PHONE_CSP_BYTE3				(0xA0) // AoC and MPTY
#define DMMTSY_PHONE_CSP_BYTE4				(0xE0) // CCBS, HOLD and WAIT
#define DMMTSY_PHONE_CSP_BYTE5				(0xFE) // all SMS services
#define DMMTSY_PHONE_CSP_BYTE6				(0x80) // ALS
#define DMMTSY_PHONE_CSP_BYTE7				(0x00) // reserved byte
#define DMMTSY_PHONE_CSP_BYTE8				(0xB3) // all CLI services
#define DMMTSY_PHONE_CSP_BYTE9				(0xC4) // GPRS, HSCSD and multiple band
#define DMMTSY_PHONE_CSP_BYTE10				(0x8C) // PLMN mode, data, fax

#define DMMTSY_PHONE_SERVICE_TABLE			(RMobilePhone::EUSIMServiceTable)
#define DMMTSY_PHONE_SST_BYTE1				(0x7F) // All SST services 1 to 7
#define DMMTSY_PHONE_SST_BYTE2				(0xFF) // All SST service 9 to 16				
#define DMMTSY_PHONE_SST_BYTE3				(0x03) // SST services 17,18 & 19
#define DMMTSY_PHONE_SST_BYTE4				(0xFF) // All SST services 25 to 32
#define DMMTSY_PHONE_SST_BYTE5				(0x34) // SST services 35, 37 & 38
#define DMMTSY_PHONE_SST_BYTE6              (0x27) // SST services 41 to 48
#define DMMTSY_PHONE_SST_BYTE7              (0x03) // SST services 49 to 56
#define DMMTSY_PHONE_SST_BYTE8              (0x7C) // SST services 57 to 64
#define DMMTSY_PHONE_SST_BYTE9              (0x3A) // SST services 65 to 72


#define DMMTSY_PHONE_BATTERY_CAPS			(TUint32)(RMobilePhone::KCapsGetBatteryInfo |\
											 RMobilePhone::KCapsNotifyBatteryInfoChange)

#define DMMTSY_PHONE_BATTERY_STATUS1		(RMobilePhone::EPoweredByBattery)
#define DMMTSY_PHONE_BATTERY_CHARGE1		(55)
#define DMMTSY_PHONE_BATTERY_STATUS2		(RMobilePhone::ENoBatteryConnected)
#define DMMTSY_PHONE_BATTERY_CHARGE2		(10)

#define DMMTSY_PHONE_SIGNAL_CAPS			(TUint32)(RMobilePhone::KCapsGetSignalStrength |\
											 RMobilePhone::KCapsNotifySignalStrengthChange)

#define DMMTSY_PHONE_SIGNAL_STRENGTH1		(-109)  // signal strength in dBm
#define DMMTSY_PHONE_SIGNAL_BAR1			(2)
#define DMMTSY_PHONE_SIGNAL_STRENGTH2		(-51)		
#define DMMTSY_PHONE_SIGNAL_BAR2			(5)

#define DMMTSY_PHONE_ACTION_INDICATOR_CAPS	(TUint32)(RMobilePhone::KCapsGetIndicator |\
											RMobilePhone::KCapsNotifyIndicatorChange)
#define DMMTSY_PHONE_INDICATOR_CAPS			(TUint32)(RMobilePhone::KIndChargerConnected |\
											RMobilePhone::KIndNetworkAvailable)
#define DMMTSY_PHONE_INDICATOR1				(TUint32)(RMobilePhone::KIndNetworkAvailable)
#define DMMTSY_PHONE_INDICATOR2				(TUint32)(RMobilePhone::KIndChargerConnected)

#define DMMTSY_PHONE_NETWORK_SELECTION_METHOD1	 RMobilePhone::ENetworkSelectionAutomatic
#define DMMTSY_PHONE_NETWORK_SELECTION_METHOD2	 RMobilePhone::ENetworkSelectionManual
#define DMMTSY_PHONE_NETWORK_SELECTION_BAND1	 RMobilePhone::ENetworkBandClassAOnly
#define DMMTSY_PHONE_NETWORK_SELECTION_BAND2	 RMobilePhone::ENetworkBandClassBOnly
#define DMMTSY_PHONE_NETWORK_SELECTION_OPERATION1	 RMobilePhone::ENetworkOperationAnalogOnly
#define DMMTSY_PHONE_NETWORK_SELECTION_OPERATION2	 RMobilePhone::ENetworkOperationDigitalPreferred

#define DMMTSY_PHONE_NOTIFY_SELECTION_SETTING	 RMobilePhone::ENetworkSelectionAutomatic
#define	DMMTSY_PHONE_MANUAL_SELECTION		 (TBool)(ETrue)
#define DMMTSY_PHONE_NETWORK_ID				  _L("MNC")
#define DMMTSY_PHONE_COUNTRY_ID				  _L("MCC")

#define DMMTSY_PHONE_NWTINFO_TIMEZONE		  0
#define DMMTSY_YEAR							  2001
#define DMMTSY_MONTH						  ESeptember
#define DMMTSY_DAY							  10
#define DMMTSY_HOUR							  16
#define DMMTSY_MIN							  56
#define DMMTSY_SEC							  05
#define DMMTSY_MICROSEC						  123
#define DMMTSY_PHONE_NWTINFO_YEAR			  2000
#define DMMTSY_PHONE_NWTINFO_MONTH			  EJuly
#define DMMTSY_PHONE_NWTINFO_DAY			  29
#define DMMTSY_PHONE_NWTINFO_HOUR			  15
#define DMMTSY_PHONE_NWTINFO_MINUTE			  59
#define DMMTSY_PHONE_NWTINFO_SECOND			  11
#define DMMTSY_PHONE_NWTINFO_MICRO			  66

#define DMMTSY_PHONE_NWTINFO_SHORT_ID         _L("Voda")
#define DMMTSY_PHONE_NWTINFO_LONG_ID          _L("Vodafone UK")
#define DMMTSY_PHONE_NWTINFO_DST              2
#define DMMTSY_PHONE_NWTINFO_NITZ_USED       (RMobilePhone::KCapsTimeAvailable |\
		                                      RMobilePhone::KCapsTimezoneAvailable |\
		                                      RMobilePhone::KCapsDSTAvailable |\
		                                      RMobilePhone::KCapsLongNameAvailable)

#define DMMTSY_PHONE_SPN_NAME				 _L("one SP")
#define DMMTSY_PHONE_PLMN_FIELD				 _L("Vodaphone UK Voda")
#define DMMTSY_PHONE_DISPREQ				 RMobilePhone::KDisplayPLMNNotRequired

#define DMMTSY_PHONE_PRIVACY_SETTING1		 RMobilePhone::EPrivacyOn
#define DMMTSY_PHONE_PRIVACY_SETTING2		 RMobilePhone::EPrivacyOff


#define DMMTSY_PHONE_IDENTITY_CAPS			(TUint32)(RMobilePhone::KCapsGetManufacturer |\
											 RMobilePhone::KCapsGetModel |\
											 RMobilePhone::KCapsGetRevision |\
											 RMobilePhone::KCapsGetSerialNumber |\
											 RMobilePhone::KCapsGetSubscriberId)

#define DMMTSY_PHONE_MANUFACTURER			_L("Symbian")
#define DMMTSY_PHONE_MODEL					_L("PearlPhone")
#define DMMTSY_PHONE_REVISION				_L("1.0")
#define DMMTSY_PHONE_SERIAL_NUMBER			_L("SYP123456789")
#define DMMTSY_PHONE_SUBSCRIBER_ID			_L("234551234567891")

#define DMMTSY_PHONE_DTMF_CAPS1				(TUint32)(RMobilePhone::KCapsSendDTMFString |\
											 RMobilePhone::KCapsSendDTMFSingleTone)
#define DMMTSY_PHONE_DTMF_CAPS2				(TUint32)(0)
#define DMMTSY_PHONE_SEND_TONES_LENGTH		(10)
#define DMMTSY_PHONE_SEND_TONES				_L("3459872356")
#define DMMTSY_PHONE_READ_TONES_LENGTH		(10)
#define DMMTSY_PHONE_READ_TONES				_L("7871283011")
#define DMMTSY_PHONE_SINGLE_TONE			'2'

#define DMMTSY_PHONE_NETWORK_CAPS			(TUint32)(0x000003FF) // all network caps
#define DMMTSY_PHONE_NETWORK_MODE1			(RMobilePhone::ENetworkModeGsm)
#define DMMTSY_PHONE_NETWORK_MODE2			(RMobilePhone::ENetworkModeCdma95)

#define DMMTSY_PHONE_HOME_NETWORK_MODE				(RMobilePhone::ENetworkModeGsm)
#define DMMTSY_PHONE_HOME_NETWORK_BAND				(RMobilePhone::E1900BandA)
#define DMMTSY_PHONE_HOME_NETWORK_STATUS			(RMobilePhone::ENetworkStatusCurrent)
#define DMMTSY_PHONE_HOME_NETWORK_MCC				_L("442")
#define DMMTSY_PHONE_HOME_NETWORK_ID				_L("12")
#define DMMTSY_PHONE_HOME_NETWORK_TAG				_L("Home: Vodafone UK")
#define DMMTSY_PHONE_HOME_NETWORK_SHORT_NAME		_L("VODA")
#define DMMTSY_PHONE_HOME_NETWORK_LONG_NAME			_L("VODAFONE UK")
#define DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH		(RMobilePhone::ENetworkAccessUtran)
#define DMMTSY_PHONE_HOME_NETWORK_HSDPA_AVAILABILITY	(TBool)(ETrue)
#define DMMTSY_PHONE_HOME_NETWORK_EGPRS_AVAILABILITY	(TBool)(ETrue)
#define DMMTSY_PHONE_HOME_NETWORK_HSUPA_AVAILABILITY	(TBool)(ETrue)
#define DMMTSY_PHONE_HOME_NETWORK_SID				_L("3456")
#define DMMTSY_PHONE_HOME_NETWORK_SEARCHINTV		(4)
#define DMMTSY_PHONE_LAST_USED_ACCESS_TECH			(RMobilePhone::ENetworkAccessUtran)
#define DMMTSY_PHONE_HOME_NETWORK_OTHER_NAMES		_L("TLVOTHERNAMESBUFFER")
#define DMMTSY_PHONE_HOME_NETWORK_NAME_LOC_ID		(1)
#define DMMTSY_PHONE_HOME_NETWORK_FIRST_ID			(43)
#define DMMTSY_PHONE_HOME_NETWORK_LAST_ID			(49)

#define DMMTSY_PHONE_HOME_NETWORK_ACCESS_TECH3		(RMobilePhone::ENetworkAccessGsmAndUtran)


#define DMMTSY_PHONE_CURRENT_NETWORK_MODE1			(RMobilePhone::ENetworkModeCdma2000)
#define DMMTSY_PHONE_CURRENT_NETWORK_STATUS1		(RMobilePhone::ENetworkStatusCurrent)
#define DMMTSY_PHONE_CURRENT_NETWORK_BAND1			(RMobilePhone::E1900BandD)
#define DMMTSY_PHONE_CURRENT_NETWORK_MCC1			_L("567")
#define DMMTSY_PHONE_CURRENT_NETWORK_ID1			_L("2335")
#define DMMTSY_PHONE_CURRENT_NETWORK_TAG1			_L("Office")
#define DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME1	_L("AT&T")
#define DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME1		_L("AT&T Wireless")
#define DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH1	(RMobilePhone::ENetworkAccessGsm)
#define DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY1 (TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY1 (TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY1 (TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_SID			_L("7849")

#define DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN		(TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_LAC1			(3465)
#define DMMTSY_PHONE_CURRENT_NETWORK_CELLID1		(99)

#define DMMTSY_PHONE_CURRENT_NETWORK_MODE2			(RMobilePhone::ENetworkModeCdma95)
#define DMMTSY_PHONE_CURRENT_NETWORK_STATUS2		(RMobilePhone::ENetworkStatusCurrent)
#define DMMTSY_PHONE_CURRENT_NETWORK_BAND2			(RMobilePhone::E800BandB)
#define DMMTSY_PHONE_CURRENT_NETWORK_MCC2			_L("398")
#define DMMTSY_PHONE_CURRENT_NETWORK_ID2			_L("1234")
#define DMMTSY_PHONE_CURRENT_NETWORK_TAG2			_L("Roaming")
#define DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME2	_L("")
#define DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME2		_L("")
#define DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_TECH2	(RMobilePhone::ENetworkAccessGsmCompact)
#define DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY2 (TBool)(EFalse)
#define DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY2 (TBool)(EFalse)
#define DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY2 (TBool)(EFalse)

#define DMMTSY_PHONE_CURRENT_NETWORK_LAC2			(8364)
#define DMMTSY_PHONE_CURRENT_NETWORK_CELLID2		(33)

#define DMMTSY_PHONE_REGISTRATION_STATUS1			(RMobilePhone::ERegisteredRoaming)
#define DMMTSY_PHONE_REGISTRATION_STATUS2			(RMobilePhone::ERegisteredOnHomeNetwork)

#define DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV		(RMobilePhone::ENetworkModeWcdma)
#define DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV		(RMobilePhone::ENetworkStatusAvailable)
#define DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV		(RMobilePhone::EBandUnknown)
#define DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV		_L("497")
#define DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV			_L("15416")
#define DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV		_L("Roaming")
#define DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV	_L("3UK")
#define DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV	_L("Three UK")
#define DMMTSY_PHONE_CURRENT_NETWORK_CDMASID_ISV					_L("12CDMA34")
#define DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_UTRAN_ISV (RMobilePhone::ENetworkAccessUtran)

#define DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV	(TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV		(925)
#define DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV	    (57)

#define DMMTSY_PHONE_CURRENT_NETWORK_MODE_ISV2		(RMobilePhone::ENetworkModeUnregistered)
#define DMMTSY_PHONE_CURRENT_NETWORK_STATUS_ISV2	(RMobilePhone::ENetworkStatusCurrent)
#define DMMTSY_PHONE_CURRENT_NETWORK_BAND_ISV2		(RMobilePhone::EBandUnknown)
#define DMMTSY_PHONE_CURRENT_NETWORK_MCC_ISV2		_L("564")
#define DMMTSY_PHONE_CURRENT_NETWORK_ID_ISV2		_L("353")
#define DMMTSY_PHONE_CURRENT_NETWORK_TAG_ISV2		_L("Search")
#define DMMTSY_PHONE_CURRENT_NETWORK_SHORT_NAME_ISV2 _L("O2")
#define DMMTSY_PHONE_CURRENT_NETWORK_LONG_NAME_ISV2	_L("O2 UK")
#define DMMTSY_PHONE_CURRENT_NETWORK_CDMASID_ISV2					_L("45CDMA67")
#define DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_UTRAN_ISV2 (RMobilePhone::ENetworkAccessJTacsStdChannels)

#define DMMTSY_PHONE_CURRENT_NETWORK_AREAKNOWN_ISV2	(TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_LAC_ISV2		(135)
#define DMMTSY_PHONE_CURRENT_NETWORK_CELLID_ISV2	(11)

#define DMMTSY_PHONE_CELL_INFO_MODE					(RMobilePhone::ECellModeGsm)
#define DMMTSY_PHONE_CELL_INFO_CELLID				(199)
#define DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE		(59)
#define DMMTSY_PHONE_CELL_INFO_STATUS				(RMobilePhone::ECellInfoStatusUnknown)

#define DMMTSY_PHONE_CELL_INFO_MODE2				(RMobilePhone::ECellModeGsm)
#define DMMTSY_PHONE_CELL_INFO_CELLID2				(300)
#define DMMTSY_PHONE_CELL_INFO_TIMING_ADVANCE2		(18)
#define DMMTSY_PHONE_CELL_INFO_STATUS2				(RMobilePhone::ECellInfoStatusCurrent)

#define DMMTSY_NETWORK_COUNT						(4)
													   
#define DMMTSY_PHONE_FEATURE_CODE					_L("73")
#define DMMTSY_PHONE_FEATURE_CODE2					_L("23")
#define DMMTSY_PHONE_NETWORK_SERVICE				(RMobilePhone::ECFNRcService)
#define DMMTSY_PHONE_NETWORK_SERVICE2				(RMobilePhone::EDeflectToVoicemail)
#define DMMTSY_PHONE_NETWORK_SERVICE_ACTION			(RMobilePhone::EServiceActionRegister)
#define DMMTSY_PHONE_NETWORK_SERVICE_ACTION2		(RMobilePhone::EServiceActionActivate)
#define DMMTSY_PHONE_NETWORK_SERVICE_REQUEST		_L("*45#012678765423#")

#define DMMTSY_PHONE_CALL_SERVICE_CAPS1	 			(TUint32)(RMobilePhone::KCapsGetCFStatusCache|\
													RMobilePhone::KCapsGetCFStatusNetwork|\
													RMobilePhone::KCapsSetCFStatus|\
													RMobilePhone::KCapsNotifyCFStatus)

#define DMMTSY_PHONE_CALL_SERVICE_CAPS2 			(TUint32)(RMobilePhone::KCapsSetCFStatus)

#define DMMTSY_PHONE_CF_CONDITION_CFU				RMobilePhone::ECallForwardingUnconditional
#define DMMTSY_PHONE_CF_CONDITION_ALL				RMobilePhone::ECallForwardingAllCases
#define DMMTSY_PHONE_CF_CONDITION_CFNRy				RMobilePhone::ECallForwardingNoReply
#define DMMTSY_PHONE_CF_LOCATION					(RMobilePhone::EInfoLocationNetwork)
#define DMMTSY_PHONE_CF_CACHELOCATION				(RMobilePhone::EInfoLocationCache)
#define DMMTSY_CF_ENTRY_COUNT						(4)
#define DMMTSY_CF_SINGLE_SERVICE_ENTRY_COUNT		(1)
#define DMMTSY_PHONE_CF_SERVICE_GROUP_VOICE			(RMobilePhone::EVoiceService)
#define DMMTSY_PHONE_CF_SERVICE_GROUP_DATA			(RMobilePhone::ECircuitDataService)
#define DMMTSY_PHONE_CF_SERVICE_GROUP_FAX			(RMobilePhone::EFaxService)
#define DMMTSY_PHONE_CF_SERVICE_GROUP_AUXVOICE		(RMobilePhone::EAuxVoiceService)
#define DMMTSY_PHONE_CF_SERVICE_GROUP_ALL			(RMobilePhone::EAllServices)
#define DMMTSY_PHONE_CF_NOT_ACTIVE					(RMobilePhone::ECallForwardingStatusNotActive)
#define DMMTSY_PHONE_CF_ACTIVE						(RMobilePhone::ECallForwardingStatusActive)
#define DMMTSY_PHONE_CF_UNKNOWN						(RMobilePhone::ECallForwardingStatusUnknown)
#define DMMTSY_PHONE_CF_TIMEOUT						(5)
#define DMMTSY_NO_CF_TIMEOUT						(-1)
#define DMMTSY_PHONE_CF_SETTING						(RMobilePhone::EServiceActionRegister)
#define DMMTSY_PHONE_CF_ACTIVE_TYPE					(RMobilePhone::ECFConditionalActive)
#define DMMTSY_PHONE_CF_PRESENTATION				(RMobilePhone::ECallForwardingPresentationAllowed)
#define DMMTSY_PHONE_CF_SCREENING					(RMobilePhone::ECallForwardingScreeningUserVerified)
#define DMMTSY_PHONE_CFU_INDICATOR						(RMobilePhone::KCFUIndicatorFax)
#define DMMTSY_PHONE_CFU_INDICATOR1					(RMobilePhone::KCFUIndicatorSms)
#define DMMTSY_PHONE_CFU_MSP						(RMobilePhone::KProfileIdentityOne)
#define DMMTSY_PHONE_CFU_TON						(RMobilePhone::ENationalNumber)
#define DMMTSY_PHONE_CFU_NPI						(RMobilePhone::ENationalNumberPlan)
#define DMMTSY_PHONE_CFU_TELNUMBER					_L("91801234567")

#define DMMTSY_PHONE_ID_SERVICE						RMobilePhone::EIdServiceCallerRestriction
#define DMMTSY_PHONE_ID_LOCATION					(RMobilePhone::EInfoLocationNetwork)
#define DMMTSY_PHONE_ID_STATUS						RMobilePhone::EIdServiceNotProvisioned
#define DMMTSY_PHONE_ID_SETTING						(RMobilePhone::EIdServicePresentationAllowed)

#define DMMTSY_PHONE_CB_CONDITION					RMobilePhone::EBarAllOutgoing
#define DMMTSY_PHONE_CB_LOCATION					(RMobilePhone::EInfoLocationNetwork)

#define	DMMTSY_CALL_BARRING_COUNT					4
#define DMMTSY_PHONE_CB_CONDITION0					RMobilePhone::EBarAllIncoming
#define DMMTSY_PHONE_CB_CONDITION1					RMobilePhone::EBarIncomingRoaming
#define DMMTSY_PHONE_CB_CONDITION2					RMobilePhone::EBarAllOutgoing
#define DMMTSY_PHONE_CB_CONDITION3					RMobilePhone::EBarOutgoingInternational
#define DMMTSY_PHONE_CB_SG0					     	RMobilePhone::EVoiceService
#define DMMTSY_PHONE_CB_SG1						    RMobilePhone::ECircuitDataService
#define DMMTSY_PHONE_CB_SG2						    RMobilePhone::EFaxService
#define DMMTSY_PHONE_CB_SG3						    RMobilePhone::EAuxVoiceService
#define DMMTSY_PHONE_CB_STATUS0					    RMobilePhone::ECallBarringStatusActive
#define DMMTSY_PHONE_CB_STATUS1						RMobilePhone::ECallBarringStatusNotActive
#define DMMTSY_PHONE_CB_STATUS2						RMobilePhone::ECallBarringStatusNotProvisioned
#define DMMTSY_PHONE_CB_STATUS3						RMobilePhone::ECallBarringStatusNotAvailable
#define DMMTSY_PHONE_CB_COMP0						(0)
#define DMMTSY_PHONE_CB_COMP1						(1)
#define DMMTSY_PHONE_CB_COMP2						(2)
#define DMMTSY_PHONE_CB_COMP3						(3)

#define DMMTSY_PHONE_CB_ACTION						RMobilePhone::EServiceActionErase
#define DMMTSY_PHONE_CB_PASSWORD					_L("noodle")
#define DMMTSY_PHONE_CB_NEW_PASSWORD				_L("pizza")
#define DMMTSY_PHONE_CB_VERIFIED_PASSWORD			_L("pizza")
#define DMMTSY_PHONE_CB_ALL_SERVICES				0
#define DMMTSY_PHONE_CB_COMP_METHOD_ID				(11)
#define DMMTSY_PHONE_CB_ALPHA_TAG					_L("ALPHA1")


#define DMMTSY_PHONE_CW_LOCATION					(RMobilePhone::EInfoLocationNetwork)
#define DMMTSY_CW_ENTRY_COUNT						4
#define DMMTSY_PHONE_CW_SG0					     	RMobilePhone::EVoiceService
#define DMMTSY_PHONE_CW_SG1						    RMobilePhone::ECircuitDataService
#define DMMTSY_PHONE_CW_SG2						    RMobilePhone::EFaxService
#define DMMTSY_PHONE_CW_SG3						    RMobilePhone::EAuxVoiceService
#define DMMTSY_PHONE_CW_STATUS0					    RMobilePhone::ECallWaitingStatusActive
#define DMMTSY_PHONE_CW_STATUS1						RMobilePhone::ECallWaitingStatusNotActive
#define DMMTSY_PHONE_CW_STATUS2						RMobilePhone::ECallWaitingStatusNotProvisioned
#define DMMTSY_PHONE_CW_STATUS3						RMobilePhone::ECallWaitingStatusNotAvailable
#define DMMTSY_PHONE_CW_SET_STATUS_ACTION			RMobilePhone::EServiceActionErase

#define DMMTSY_PHONE_CCC_STATUS						RMobilePhone::ECcbsNotProvisioned
#define DMMTSY_PHONE_CCC_LOCATION					(RMobilePhone::EInfoLocationNetwork)
#define DMMTSY_PHONE_CCC_ENTRY_COUNT				4

#define DMMTSY_PHONE_CCC_CALLNAME0					_L("call1")
#define	DMMTSY_PHONE_CCC_SERVICE_GROUP0				RMobilePhone::EVoiceService
#define DMMTSY_PHONE_CCC_INDEX0						0
#define DMMTSY_PHONE_CCC_DESTINATION_TON0			RMobilePhone::EInternationalNumber
#define DMMTSY_PHONE_CCC_DESTINATION_NP0			RMobilePhone::EIsdnNumberPlan
#define DMMTSY_PHONE_CCC_DESTINATION_NUM0			_L("12345")
#define DMMTSY_PHONE_CCC_CALLNAME1					_L("call2")
#define	DMMTSY_PHONE_CCC_SERVICE_GROUP1				RMobilePhone::ECircuitDataService
#define DMMTSY_PHONE_CCC_INDEX1						1
#define DMMTSY_PHONE_CCC_DESTINATION_TON1			RMobilePhone::ENationalNumber
#define DMMTSY_PHONE_CCC_DESTINATION_NP1			RMobilePhone::EDataNumberPlan
#define DMMTSY_PHONE_CCC_DESTINATION_NUM1			_L("12346")
#define DMMTSY_PHONE_CCC_CALLNAME2					_L("call3")
#define	DMMTSY_PHONE_CCC_SERVICE_GROUP2				RMobilePhone::EFaxService
#define DMMTSY_PHONE_CCC_INDEX2						2
#define DMMTSY_PHONE_CCC_DESTINATION_TON2			RMobilePhone::ENetworkSpecificNumber
#define DMMTSY_PHONE_CCC_DESTINATION_NP2			RMobilePhone::ETelexNumberPlan
#define DMMTSY_PHONE_CCC_DESTINATION_NUM2			_L("12347")
#define DMMTSY_PHONE_CCC_CALLNAME3					_L("call4")
#define	DMMTSY_PHONE_CCC_SERVICE_GROUP3				RMobilePhone::EAuxVoiceService
#define DMMTSY_PHONE_CCC_INDEX3						3
#define DMMTSY_PHONE_CCC_DESTINATION_TON3			RMobilePhone::ESubscriberNumber
#define DMMTSY_PHONE_CCC_DESTINATION_NP3			RMobilePhone::EIsdnNumberPlan
#define DMMTSY_PHONE_CCC_DESTINATION_NUM3			_L("12348")
#define DMMTSY_PHONE_CCC_DESTINATION_CCBS_RECEIVED	(RMobilePhone::ECcbsRecallReceived)

#define DMMTSY_PHONE_ALTERNATING_CALL_CAPS			(RMobilePhone::KCapsMOVoiceData | RMobilePhone::KCapsMTVoiceThenData)
#define DMMTSY_PHONE_ALTERNATING_CALL_MODE			(RMobilePhone::EAlternatingModeVoiceData)
#define DMMTSY_PHONE_ALTERNATING_CALL_SERVICE		RMobilePhone::EAllServices

#define DMMTSY_PHONE_ALS_LINE						(RMobilePhone::EAlternateLineUnknown)

#define DMMTSY_PHONE_COST_CAPS						(RMobilePhone::KCapsSetPuct | RMobilePhone::KCapsGetCost | RMobilePhone::KCapsCostInformation)
#define DMMTSY_PHONE_COST_UNITS						100
#define DMMTSY_PHONE_COST_SERVICE					(RMobilePhone::ECostServiceInformation)
#define DMMTSY_PHONE_COST_CCM						10
#define DMMTSY_PHONE_COST_ACM						1000
#define DMMTSY_PHONE_COST_ACMMAX					10000
#define DMMTSY_PHONE_COST_PUCT_CNAME				_L("STG")
#define DMMTSY_PHONE_COST_PUCT_PPU					5
#define DMMTSY_PHONE_COST_METER						RMobilePhone::EClearACM

#define DMMTSY_PHONE_SECURITY_CAPS					(RMobilePhone::KCapsLockICC || RMobilePhone::KCapsAccessPin1)
#define DMMTSY_PHONE_SECURITY_LOCK					(RMobilePhone::ELockICC)
#define DMMTSY_PHONE_SECURITY_STATUS				(RMobilePhone::EStatusUnlocked)
#define DMMTSY_PHONE_SECURITY_SETTING				(RMobilePhone::ELockSetDisabled)

#define DMMTSY_PHONE_SECURITY_SC_LOCK				(RMobilePhone::ELockPhoneToICC)
#define DMMTSY_PHONE_SECURITY_SC_SETTING			(RMobilePhone::ELockSetEnabled)
#define DMMTSY_PHONE_SECURITY_OLD_PASS				_L("Old")
#define DMMTSY_PHONE_SECURITY_NEW_PASS				_L("New")
#define DMMTSY_PHONE_SECURITY_CODE					(RMobilePhone::ESecurityCodePhonePassword)
#define DMMTSY_PHONE_SECURITY_EVENT					(RMobilePhone::EPin1Required)
#define DMMTSY_PHONE_SECURITY_REMAINING_ATTEMPTS	3

#define DMMTSY_PHONE_MW_COUNT						1
#define DMMTSY_PHONE_VOICEMSGS						3
#define DMMTSY_PHONE_VOICEMSGS2						0
#define DMMTSY_PHONE_AUXVOICEMSGS					1
#define DMMTSY_PHONE_AUXVOICEMSGS2					2
#define DMMTSY_PHONE_DATAMSGS						4
#define DMMTSY_PHONE_DATAMSGS2						5
#define DMMTSY_PHONE_FAXMSGS						1
#define DMMTSY_PHONE_FAXMSGS2						0
#define DMMTSY_PHONE_EMAILMSGS						2
#define DMMTSY_PHONE_EMAILMSGS2						3
#define DMMTSY_PHONE_OTHERMSGS						4
#define DMMTSY_PHONE_OTHERMSGS2						8
#define DMMTSY_PHONE_VIDEOMSGS1						9
#define DMMTSY_PHONE_VIDEOMSGS2						5
#define DMMTSY_PHONE_DISPLAYSTATUS					0x1f

#define DMMTSY_PHONE_FDN_STATUS						(RMobilePhone::EFdnActive)
#define DMMTSY_PHONE_FDN_SETTING					(RMobilePhone::EFdnSetOn)

#define DMMTSY_PHONE_USER_MAX_BEARERS1              3
#define DMMTSY_PHONE_PROVIDER_MAX_BEARERS1          4
#define DMMTSY_PHONE_NETWORK_MAX_BEARERS1           5
#define DMMTSY_PHONE_UE_MAX_BEARERS1                6

#define DMMTSY_PHONE_USER_MAX_BEARERS2              4
#define DMMTSY_PHONE_PROVIDER_MAX_BEARERS2          5
#define DMMTSY_PHONE_NETWORK_MAX_BEARERS2           6
#define DMMTSY_PHONE_UE_MAX_BEARERS2                7
#define DMMTSY_PHONE_MULTICALL_IND				(RMobilePhone::EBearerUserExceeded)

#define DMMTSY_PHONE_INCOMING_CALL_TYPES            (RMobilePhone::EIncomingVoice)
#define DMMTSY_PHONE_INCOMING_CALL_TYPES1           (RMobilePhone::EIncomingData)

#define DMMTSY_PHONE_UUS_SETTING1                   (RMobilePhone::EIncomingUUSAccepted)
#define DMMTSY_PHONE_UUS_SETTING2                   (RMobilePhone::EIncomingUUSRejected)

#define DMMTSY_PHONE_MM_SETTING						(RMobilePhone::EAcceptMMCallsVoiceFallback)
#define DMMTSY_PHONE_MM_SETTING_2					(RMobilePhone::EAcceptMMCallsNoFallback)

#define DMMTSY_PHONE_NET_SECURITY					(RMobilePhone::ECipheringWCDMA)
#define DMMTSY_PHONE_NET_CIPHER_IND					(RMobilePhone::ECipheringDisplayOn)


#define DMMTSY_PHONE_USIM_AID						_L8("Active")
#define DMMTSY_PHONE_USIM_COUNT						10
#define DMMTSY_PHONE_USIM_APPINDEX					2
#define DMMTSY_PHONE_USIM_APPLABEL					_L8("L")
#define DMMTSY_PHONE_USIM_SELMODE					(RMobilePhone::EUSimSelectionAutomatic)
#define DMMTSY_PHONE_USIM_ACTIVATE					(RMobilePhone::EUSimActivate)

#define DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSM			  (RMobilePhone::KNetworkAccessGsm)
#define DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_GSMC			  (RMobilePhone::KNetworkAccessGsmCompact)
#define DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_UTRAN			  (RMobilePhone::KNetworkAccessUtran)
#define DMMTSY_PHONE_NTWK_LIST_ACCESS_CAPS_WLAN			  (RMobilePhone::KNetworkAccessWLAN)

#define DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_TRUE		  (TBool)(ETrue)
#define DMMTSY_PHONE_NTWK_LIST_USER_DEFINED_FALSE		  (TBool)(EFalse)
#define DMMTSY_PHONE_NTWK_LIST_NETWORK_ID1				  _L("MNC1")
#define DMMTSY_PHONE_NTWK_LIST_NETWORK_ID2				  _L("MNC2")
#define DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID1				  _L("MCC1")
#define DMMTSY_PHONE_NTWK_LIST_COUNTRY_ID2				  _L("MCC2")
#define DMMTSY_PHONE_NTWK_LIST_ENTRIES						(4)

//testing smart card application APIs

#define DMMTSY_PHONE_SC_COUNT 						(6)
#define DMMTSY_PHONE_SC_APP_INDEX1 					(1)
#define DMMTSY_PHONE_SC_APP_INDEX2 					(2)
#define DMMTSY_PHONE_SC_AID							_L8("ABCDEFGHIJKLMNOP")
#define DMMTSY_PHONE_SC_LABEL						_L8("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456")
#define DMMTSY_PHONE_SC_OUTOFRANGE					(KErrTooBig)
#define DMMTSY_PHONE_SC_EVENT_ACTIVATE				(RMobilePhone::EScApplicationActivated)
#define DMMTSY_PHONE_SC_AID2						_L8("AID2")
#define	DMMTSY_PHONE_SC_ACTION_TERMINATE			(RMobilePhone::EScTerminate)
#define DMMTSY_PHONE_SC_PATH_REF					_L8("7FFF6F07")
#define DMMTSY_PHONE_SC_FILEID						(0x6F07)
#define DMMTSY_PHONE_SC_NORECORDS					(3)
#define DMMTSY_PHONE_SC_RECORD_SIZE					(5)
#define DMMTSY_PHONE_SC_FILESIZE					(DMMTSY_PHONE_SC_RECORD_SIZE*DMMTSY_PHONE_SC_NORECORDS)
#define DMMTSY_PHONE_SC_TYPE						(RMobilePhone::ELinearFixed)				
#define DMMTSY_PHONE_SC_START_OFFSET				(21)
#define DMMTSY_PHONE_SC_RECORD0						_L8("\x01\x02\x03\x04\x05")
#define DMMTSY_PHONE_SC_RECORD1						_L8("\x0A\x0B\x0C\x0D\x0E")
#define DMMTSY_PHONE_SC_RECORD2						_L8("\x31\x32\x33\x34\x35")

// Testing Mobile Call

#define DMMTSY_CALL_CUG_INVOKE						(TBool)(ETrue)
#define DMMTSY_CALL_CUG_INDEX						(23)
#define DMMTSY_CALL_CUG_OA							(TBool)(EFalse)
#define DMMTSY_CALL_CUG_PREF						(TBool)(ETrue)

#define DMMTSY_DATA_CALL_SERVICE					(RMobileCall::EServiceDataCircuitAsyncRdi)
#define DMMTSY_DATA_CALL_SPEED						(RMobileCall::ESpeed9600)
#define DMMTSY_DATA_CALL_PROTOCOL					(RMobileCall::EProtocolV110)
#define DMMTSY_DATA_CALL_QOS						(RMobileCall::EQoSNonTransparent)
#define DMMTSY_DATA_CALL_RLP                        (RMobileCall::ERLPSingleLinkVersion1)
#define DMMTSY_DATA_CALL_V42BIS                     (RMobileCall::EV42bisBothDirections)

#define DMMTSY_DATA_CALL_EGPRS_REQUIRED             (TBool)(ETrue)
#define DMMTSY_DATA_CALL_EGPRS_NOTREQUIRED          (TBool)(EFalse)

#define DMMTSY_DATA_CALL_SERVICE1					(RMobileCall::EServiceDataCircuitSync)
#define DMMTSY_DATA_CALL_SPEED1						(RMobileCall::ESpeed48000)
#define DMMTSY_DATA_CALL_PROTOCOL1					(RMobileCall::EProtocolPstnMultimediaVoiceFallback)
#define DMMTSY_DATA_CALL_QOS1						(RMobileCall::EQosTransparentPreferred)
#define DMMTSY_DATA_CALL_RLP1                       (RMobileCall::ERLPMultiLinkVersion2)
#define DMMTSY_DATA_CALL_V42BIS1                    (RMobileCall::EV42bisRxDirection)

#define DMMTSY_CALL_DATA_CAPS_SPEED					(RMobileCall::KCapsSpeed2400 | RMobileCall::KCapsSpeed4800 | RMobileCall::KCapsSpeed9600)
#define DMMTSY_CALL_DATA_CAPS_CARRIER				(RMobileCall::KCapsProtocolV32)
#define DMMTSY_CALL_DATA_CAPS_SERVICE				(RMobileCall::KCapsDataCircuitAsynchronous | RMobileCall::KCapsPacketAccessSyncUDI)
#define DMMTSY_CALL_DATA_CAPS_HSCSD					(TBool)(ETrue)
#define DMMTSY_CALL_DATA_CAPS_MCLASS				1
#define DMMTSY_CALL_DATA_CAPS_MAXRXTS				5
#define DMMTSY_CALL_DATA_CAPS_MAXTXTS				5
#define DMMTSY_CALL_DATA_CAPS_TOTRXTXTS				10
#define DMMTSY_CALL_DATA_CAPS_CODING				(RMobileCall::KCapsAiurCoding48 | RMobileCall::KCapsAiurCoding96)
#define DMMTSY_CALL_DATA_CAPS_ASYMETRY              (RMobileCall::EAsymmetryUplink)
#define DMMTSY_CALL_DATA_CAPS_USERIMI               (TBool)(ETrue)

#define DMMTSY_CALL_DATA_RLP_WSMAX                  10
#define DMMTSY_CALL_DATA_RLP_WSMIN                   5
#define DMMTSY_CALL_DATA_RLP_MWSMAX                  8
#define DMMTSY_CALL_DATA_RLP_MWSMIN                  4
#define DMMTSY_CALL_DATA_RLP_N2MAX                   4
#define DMMTSY_CALL_DATA_RLP_N2MIN                   2
#define DMMTSY_CALL_DATA_RLP_T1MAX                  12
#define DMMTSY_CALL_DATA_RLP_T1MIN                   1
#define DMMTSY_CALL_DATA_RLP_T4MAX                  10
#define DMMTSY_CALL_DATA_RLP_T4MIN                   2

#define DMMTSY_CALL_RLP_VERSION1                     2

#define DMMTSY_CALL_HCSD_AIUR						RMobileCall::EAiurBps9600
#define DMMTSY_CALL_HCSD_TS							5
#define DMMTSY_CALL_HCSD_MAXTS						10
#define DMMTSY_CALL_HCSD_CODING     				RMobileCall::ETchCoding48
#define DMMTSY_CALL_HCSD_ASYMMETRY                  RMobileCall::EAsymmetryDownlink
#define DMMTSY_CALL_HCSD_USER_IMI                   (TBool)(ETrue)


#define DMMTSY_CALL_CONTROL_CAPS1					(TUint32)(0x0000000C) // call is a voice call and can be dialled
#define DMMTSY_CALL_CONTROL_CAPS2					(TUint32)(0x00000041) // call is an incoming data call
#define DMMTSY_CALL_EVENT_CAPS1						(TUint32)(0x0000007F) // all call events supported
#define DMMTSY_CALL_EVENT_CAPS2						(TUint32)(0x0000000F) // local call events supported

#define DMMTSY_CALL_STATUS1							RMobileCall::EStatusHold
#define DMMTSY_CALL_STATUS2							RMobileCall::EStatusDialling
#define DMMTSY_CALL_STATUS3							RMobileCall::EStatusIdle
#define DMMTSY_CALL_STATUS4							RMobileCall::EStatusConnected

#define DMMTSY_CALL_EVENT1							(RMobileCall::ELocalResume)

#define DMMTSY_CALL_INFORMATION_CAPS				(TUint32)(0x0000003F) // all call information supported

#define DMMTSY_VOICE_FALLBACK_CALL_NAME				_L("VoiceCall1")

#define DMMTSY_CALL_ID1								(3)
#define DMMTSY_CALL_INTERVAL						(8)
#define DMMTSY_CALL_AUTO_REDIAL_FLAG				(TBool)(ETrue)
#define DMMTSY_CALL_NEW_BEARER						(RMobileCall::EMulticallNewBearer)

#define DMMTSY_CALL_PARAM_ORIGIN					(RMobileCall::EOriginatorSIM)
#define DMMTSY_CALL_SUBADDRESS						_L("A subaddress 12345678")
#define DMMTSY_CALL_BEARER_CAP1						_L8("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E")
#define DMMTSY_CALL_BEARER_CAP2						_L8("\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00")
#define DMMTSY_CALL_BC_REPEAT_INDICATOR				(RMobileCall::EBCServiceChangeAndFallbackMode)
#define DMMTSY_CALL_ICON_ID_QUALIFIER				(RMobileCall::ESelfExplanatory)
#define DMMTSY_CALL_ICON_ID_IDENTIFIER				(3)
#define DMMTSY_CALL_ALPHA_ID						_L("Alpha ID")

#define DMMTSY_CALL_REMOTEPARTYSTATUS1				(RMobileCall::ERemoteIdentityAvailable)
#define DMMTSY_CALL_REMOTEPARTYSTATUS2				(RMobileCall::ERemoteIdentitySuppressed)
#define DMMTSY_CALL_REMOTEPARTYNUMBER1				DMMTSY_PHONE_TEL_NUMBER
#define DMMTSY_CALL_REMOTEPARTYDIRECTION1			(RMobileCall::EMobileTerminated)
#define DMMTSY_CALL_REMOTEPARTYDIRECTION2			(RMobileCall::EMobileOriginated)
#define DMMTSY_CALL_REMOTEPARTYNAME					_L("Katie")
#define DMMTSY_CALL_EMPTY_STRING					_L("")

#define DMMTSY_CALL_EXIT_CODE						(TInt)(0xEF4EFFFE) // extended error + core error
#define DMMTSY_CALL_EMERGENCY_FLAG					(TBool)(EFalse)
#define DMMTSY_CALL_FORWARDED_FLAG					(TBool)(ETrue)
#define DMMTSY_CALL_SERVICE							RMobilePhone::ECircuitDataService
#define DMMTSY_CALL_VALID_FLAG						(TBool)(ETrue)
#define DMMTSY_CALL_PRIVACY							(RMobilePhone::EPrivacyOn)
#define DMMTSY_CALL_CHANNEL							(RMobileCall::ETchDigital)
#define DMMTSY_CALL_ALTERNATING						(RMobilePhone::EAlternatingModeSingle)
#define DMMTSY_CALL_SET_PRIVACY						(RMobilePhone::EPrivacyOff)
#define DMMTSY_CALL_TCH_REQUEST						(RMobileCall::ETchAnalog)
#define DMMTSY_CALL_TCH_SETTING						(RMobileCall::ETchDigital)
#define DMMTSY_CALL_SECURITY						(RMobilePhone::ECipheringWCDMA)
#define DMMTSY_CALL_ORIGIN							(RMobileCall::EOriginatorEtelClient)
#define DMMTSY_CALL_PARAMS_CALL_CONTROL_MODIFIED	(TBool)(ETrue)

#define DMMTSY_CALL_DEFLECT_TYPE					(RMobileCall::EDeflectSuppliedNumber)

#define DMMTSY_NUMBER_OF_SLOTS_STATUS				(3)

#define DMMTSY_CALL_CLI_RESTRICT_SETTING1	        RMobileCall::EDontSendMyId

#define DMMTSY_CALL_UUS_CAPS                        (RMobileCall::KCapsSetupUUS1Implicit | RMobileCall::KCapsSetupUUS3 | RMobileCall::KCapsActiveUUS2)
#define DMMTSY_CALL_UUI_INFO                         _L("With regards to last meeting")
#define DMMTSY_CALL_UUI_INFO_DIAL                    _L("Additional info")
#define DMMTSY_CALL_UUI_INFO_HANGUP                  _L("My business card")
#define DMMTSY_CALL_UUI_INFO_ANSWER                  _L("Deal accepted")
#define DMMTSY_CALL_UUI_MORE_INFO                   (TBool)(ETrue)
#define DMMTSY_CALL_UUI_SERVICE_REQ                 (RMobileCall::KUUS1Implicit | RMobileCall::KUUS2Required)

// Testing SMS Messaging
// Testing SMS store

#define DMMTSY_PHONE_MESSAGE_STORE_COUNT			(2)
#define DMMTSY_SMS_STORE_COUNT						(4)

#define DMMTSY_SMS_STORE_INDEX1						(30)
#define DMMTSY_SMS_STORE_INDEX2						(45)
#define DMMTSY_SMS_STORE_INDEX3						(101)
#define DMMTSY_SMS_STORE_INDEX4						(127)

#define DMMTSY_SMS_MESSAGING_MODE_CAPS				(TUint32)(RMobileSmsMessaging::KCapsGsmSms |\
													RMobileSmsMessaging::KCapsCdmaSms)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS			(TUint32)(RMobileSmsMessaging::KCapsReceiveStored |\
													RMobileSmsMessaging::KCapsReceiveUnstoredClientAck |\
													RMobileSmsMessaging::KCapsSendNoAck |\
													RMobileSmsMessaging::KCapsSendWithAck)

#define DMMTSY_SMS_MESSAGING_RECEIVE_MODE1			RMobileSmsMessaging::EReceiveUnstoredClientAck
#define DMMTSY_SMS_MESSAGING_RECEIVE_MODE2			RMobileSmsMessaging::EReceiveStored

#define DMMTSY_SMS_MESSAGING_BEARER1				RMobileSmsMessaging::ESmsBearerPacketPreferred
#define DMMTSY_SMS_MESSAGING_BEARER2				RMobileSmsMessaging::ESmsBearerCircuitOnly

#define DMMTSY_SMS_MESSAGE_FLAGS2					(RMobileSmsMessaging::KSmsDataFormat |\
													 RMobileSmsMessaging::KRemotePartyInfo |\
													 RMobileSmsMessaging::KMoreToSend)

#define DMMTSY_SMS_MESSAGE_MORE						(TBool)(ETrue)
#define DMMTSY_SMS_MESSAGE_FULL						(TBool)(ETrue)

#define DMMTSY_SMS_MESSAGE_FLAGS1					(RMobileSmsMessaging::KSmsDataFormat |\
													 RMobileSmsMessaging::KIncomingStatus |\
													 RMobileSmsMessaging::KStorageLocation |\
													 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_MESSAGE_FORMAT1					RMobileSmsMessaging::EFormatGsmTpdu
#define DMMTSY_SMS_MESSAGE_SC1						DMMTSY_PHONE_TEL_NUMBER
#define DMMTSY_SMS_MESSAGE_PDU1						_L8("11111234567899876543211111")
#define DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1		DMMTSY_PHONE_TEL_NUMBER
#define DMMTSY_SMS_MESSAGE_STATUS1					RMobileSmsMessaging::EMtMessageStored
#define DMMTSY_SMS_MESSAGE_TELESERVICE				(34)

#define DMMTSY_SMS_MESSAGE_PDU2						_L8("22221234567899876543212222")
#define DMMTSY_SMS_MESSAGE_FORMAT2					RMobileSmsMessaging::EFormatGsmTpduScPrefix
#define DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER2		_L("")
#define DMMTSY_SMS_MESSAGE_STATUS2					RMobileSmsMessaging::EMtMessageUnstoredClientAck

#define DMMTSY_SMS_MESSAGE_NACK_CAUSE				(-4534) // KErrGsmSMSMemoryCapacityExceeded
#define DMMTSY_SMS_MESSAGE_MSGREF					(3405)
#define DMMTSY_SMS_MESSAGE_CDMA_FLAGS				(RMobileSmsMessaging::KSmsDataFormat |\
													 RMobileSmsMessaging::KRemotePartyInfo |\
													 RMobileSmsMessaging::KIncomingStatus |\
													 RMobileSmsMessaging::KStorageLocation |\
													 RMobileSmsMessaging::KMessageReference |\
													 RMobileSmsMessaging::KCdmaTeleservice |\
													 RMobileSmsMessaging::KCdmaServiceCategory |\
													 RMobileSmsMessaging::KCdmaTLAckRequired |\
													 RMobileSmsMessaging::KCdmaAddressAuthentication)
#define DMMTSY_SMS_MESSAGE_CDMA_FORMAT				RMobileSmsMessaging::EFormatCdmaTpdu
#define DMMTSY_SMS_MESSAGE_CDMA_TELESERVICE			(333)
#define DMMTSY_SMS_MESSAGE_CDMA_SERVICE_CATEGORY	(42)
#define DMMTSY_SMS_MESSAGE_ADDRESS_AUTHENTICATION	(0x00ffffff)
#define DMMTSY_SMS_MESSAGE_TLACK					_L8("33331234567899876543213333")
#define DMMTSY_SMS_MESSAGE_TON						(RMobilePhone::ENetworkSpecificNumber)
#define DMMTSY_SMS_MESSAGE_NP						(RMobilePhone::EPrivateNumberPlan)
#define DMMTSY_MESSAGE_STORE_NAME					(KETelIccSmsStore)
#define DMMTSY_MESSAGE_STORE_TYPE					(RMobilePhoneStore::EShortMessageStore)
#define DMMTSY_MESSAGE_STORE_TOTAL_ENTRIES			(100)
#define DMMTSY_MESSAGE_STORE_USED_ENTRIES			(23)
#define DMMTSY_MESSAGE_STORE_CAPS					(TUint32)(RMobilePhoneStore::KCapsWholeStore |\
													RMobilePhoneStore::KCapsIndividualEntry)
#define DMMTSY_MESSAGE_STORE_EVENT					RMobilePhoneStore::KStoreHasSpace

#define DMMTSY_SMS_MESSAGE_STORE_STATUS				RMobileSmsMessaging::EMessageRead
#define DMMTSY_SMS_STORE_STATUS1					RMobileSmsStore::EStoredMessageUnread
#define DMMTSY_SMS_STORE_STATUS2					RMobileSmsStore::EStoredMessageSent
#define DMMTSY_SMS_STORE_BATCH						2
#define DMMTSY_SMS_STORE_START						1

#define DMMTSY_SMS_STORE_GSMENTRY_TYPE				RMobileSmsStore::KETelMobileGsmSmsEntryV1
#define DMMTSY_SMS_STORE_CDMAENTRY_TYPE				RMobileSmsStore::KETelMobileCdmaSmsEntryV1

#define DMMTSY_SMSP_STORE_SCA						_L("02075474324")
#define DMMTSY_SMSP_STORE_EVENT				        RMobilePhoneStore::EStoreFull
#define DMMTSY_SMSP_STORE_INDEX1					(1)
#define DMMTSY_SMSP_STORE_TEXT1						_L("Katie's SMS address")
#define DMMTSY_SMSP_STORE_VALID_PARAMS1				(TUint8)(0x1F)
#define DMMTSY_SMSP_STORE_PID1						(TUint8)(0x34)
#define DMMTSY_SMSP_STORE_DCS1						(TUint8)(0x8A)
#define DMMTSY_SMSP_STORE_VALIDITY1					(TUint8)(0x91)

#define DMMTSY_SMSP_STORE_INDEX2					(2)
#define DMMTSY_SMSP_STORE_TEXT2						_L("Ian's SMS address")
#define DMMTSY_SMSP_STORE_VALID_PARAMS2				(TUint8)(0x1F)
#define DMMTSY_SMSP_STORE_PID2						(TUint8)(0x73)
#define DMMTSY_SMSP_STORE_DCS2						(TUint8)(0xF2)
#define DMMTSY_SMSP_STORE_VALIDITY2					(TUint8)(0x04)

#define DMMTSY_SMSP_STORE_STATUS					(TBool)(EFalse)
#define DMMTSY_SMSP_STORE_COUNT						(2)

// Testing Broadcast Messaging

#define DMMTSY_BROADCAST_MESSAGING_MODE_CAPS		(TUint32)(RMobileBroadcastMessaging::KCapsGsmTpduFormat)
#define DMMTSY_BROADCAST_MESSAGING_FILTER_CAPS		(TUint32)(RMobileBroadcastMessaging::KCapsLangFilter |\
															  RMobileBroadcastMessaging::KCapsIdFilter )
#define DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS        (TUint32)(RMobileBroadcastMessaging::KBroadcastDataFormat)
#define DMMTSY_BROADCAST_RECEIVE_SETTING1			RMobileBroadcastMessaging::EBroadcastAcceptAll
#define DMMTSY_BROADCAST_RECEIVE_SETTING2			RMobileBroadcastMessaging::EBroadcastAcceptFilter
#define DMMTSY_CBS_MESSAGE_PDU						_L8("227437498357221234567899876543212222")
#define DMMTSY_CBS_MESSAGE_PDU_FORMAT				RMobileBroadcastMessaging::EFormatGsmTpdu
#define DMMTSY_BROADCAST_ID_TYPE					(RMobileBroadcastMessaging::EGsmBroadcastId)

#define DMMTSY_BROADCAST_MESSAGING_ATT_DCS			1
#define DMMTSY_BROADCAST_MESSAGING_ATT_MSGID		1
#define DMMTSY_BROADCAST_MESSAGING_ATT_MSGTYPE		1
#define DMMTSY_BROADCAST_MESSAGING_ATT_NUMOFPAGES   1
#define DMMTSY_BROADCAST_MESSAGING_ATT_SERIAL		123

// Note this string does not represent an authentic language filter string

#define DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER			_L("ENGRFR")

#define DMMTSY_CBMI_STORE_COUNT						(3)
#define DMMTSY_CBMI_STORE_INDEX1					(1)
#define DMMTSY_CBMI_ID1								(TUint16)(0x34F7)
#define DMMTSY_CBMI_STORE_INDEX2					(2)
#define DMMTSY_CBMI_ID2								(TUint16)(0x985D)
#define DMMTSY_CBMI_STORE_INDEX3					(3)
#define DMMTSY_CBMI_ID3								(TUint16)(0x0065)	
#define DMMTSY_CBMI_MAX_ENTRIES							10
	
// Testing USSD Messaging

#define DMMTSY_USSD_MESSAGING_TYPE_CAPS				(TUint32)(RMobileUssdMessaging::KCapsMOUssd |\
													RMobileUssdMessaging::KCapsMTUssd)
#define DMMTSY_USSD_ATTRIBUTE_FLAGS					(RMobileUssdMessaging::KUssdDataFormat |\
													 RMobileUssdMessaging::KUssdMessageType |\
													 RMobileUssdMessaging::KUssdMessageDcs)
#define DMMTSY_USSD_ATTRIBUTE_FORMAT				RMobileUssdMessaging::EFormatPackedString
#define DMMTSY_USSD_ATTRIBUTE_TYPE					RMobileUssdMessaging::EUssdMORequest
#define DMMTSY_USSD_ATTRIBUTE_DCS					(0x38)

#define DMMTSY_USSD_MESSAGE_PDU						_L8("227437498357221fjdfjlkfjdsfiw234567899876543212222")

#define DMMTSY_NAM_STORE_NAME						KETelNamStore
#define DMMTSY_NAM_STORE_TYPE						RMobilePhoneStore::ENamStore
#define DMMTSY_NAM_STORE_ENTRIES					10
#define DMMTSY_NAM_STORE_USED						5
#define DMMTSY_NAM_STORE_CAPS						(RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess)
#define DMMTSY_NAM_STORE_COUNT						2
#define DMMTSY_NAM_STORE_ACTIVE						3

#define DMMTSY_NAM_STORE_INDEX1						1
#define DMMTSY_NAM_STORE_INDEX2						2
#define DMMTSY_NAM_STORE_NAMID						2
#define DMMTSY_NAM_STORE_PARAM						4
#define DMMTSY_NAM_STORE_DATA						_L8("Hello I'm a Nam Entry")
#define DMMTSY_NAM_STORE_DATA_V4					_L8("Hello I'm a Nam Entry of size 256 bytes (the maximum length for a V4 NAM entry).12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456")
#define DMMTSY_NAM_STORE_EVENT						RMobilePhoneStore::KStoreEntryDeleted

#define DMMTSY_ON_STORE_NAME						KETelOwnNumberStore
#define DMMTSY_ON_STORE_TYPE						RMobilePhoneStore::EOwnNumberStore
#define DMMTSY_ON_STORE_ENTRIES						8
#define DMMTSY_ON_STORE_USED						5
#define DMMTSY_ON_STORE_CAPS						STATIC_CAST(TUint32, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsWholeStore))

#define DMMTSY_ON_STORE_INDEX1						1
#define DMMTSY_ON_STORE_INDEX2						2
#define DMMTSY_ON_STORE_ENTRY_MODE					RMobilePhone::ENetworkModeGsm
#define DMMTSY_ON_STORE_ENTRY_TEXT					_L("Data Number")
#define DMMTSY_ON_STORE_ENTRY_SERVICE				RMobilePhone::ECircuitDataService
#define DMMTSY_ON_STORE_INFO_GSM_MAX_NUM			10		
#define DMMTSY_ON_STORE_INFO_GSM_MAX_TEXT			20
#define DMMTSY_ON_STORE_EVENT						RMobilePhoneStore::KStoreEntryAdded
#define DMMTSY_ON_STORE_COUNT						2

#define	DMMTSY_EN_STORE_COUNT							2
#define DMMTSY_EN_STORE_INDEX1							1
#define DMMTSY_EN_STORE_INDEX2							2
#define DMMTSY_EN_STORE_NAME							KETelEmergencyNumberStore
#define DMMTSY_EN_STORE_TYPE							RMobilePhoneStore::EEmergencyNumberStore
#define DMMTSY_EN_STORE_ENTRIES							1
#define DMMTSY_EN_STORE_USED							0
#define DMMTSY_EN_STORE_CAPS							STATIC_CAST(TUint32, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsWholeStore))
#define DMMTSY_EN_STORE_NS								(TBool)(EFalse)
#define DMMTSY_EN_STORE_MODE							RMobilePhone::ENetworkModeCdma95
#define DMMTSY_EN_STORE_CC								_L("UK")
#define DMMTSY_EN_STORE_ID								_L("ME")
#define DMMTSY_EN_STORE_NUM								_L("999")
#define DMMTSY_EN_STORE_EVENT							RMobilePhoneStore::KStoreEmpty

#define DMMTSY_EN_STORE_AMBULANCE_NUM                   _L("1459")
#define DMMTSY_EN_STORE_AMBULANCE_ID                    _L("Ambulance")
#define DMMTSY_EN_STORE_CALL_TYPE                       0

#define	DMMTSY_PHONEBOOK_STORE_COUNT					2
#define DMMTSY_PHONEBOOK_STORE_INDEX1					1
#define DMMTSY_PHONEBOOK_STORE_INDEX2					2
#define DMMTSY_PHONEBOOK_STORE_NAME						KETelIccVoiceMailBox
#define DMMTSY_PHONEBOOK_STORE_TYPE						RMobilePhoneStore::EEmergencyNumberStore
#define DMMTSY_PHONEBOOK_STORE_ENTRIES					1
#define DMMTSY_PHONEBOOK_STORE_USED						0
#define DMMTSY_PHONEBOOK_STORE_CAPS						STATIC_CAST(TUint32, (RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsWholeStore))
#define DMMTSY_PHONEBOOK_TEXT							_L("Mark's Mobile")
#define DMMTSY_PHONEBOOK_STORE_EVENT					RMobilePhoneStore::EStoreFull
#define DMMTSY_PHONEBOOK_MAXNL							10
#define DMMTSY_PHONEBOOK_MAXTL							140
#define DMMTSY_PHONEBOOK_LOCATION						RMobilePhoneBookStore::ELocationIccMemory
#define DMMTSY_PHONEBOOK_IDENTITY						_L8("1215559294")
#define DMMTSY_PHONEBOOK_CHANGECOUNT					12
#define DMMTSY_PHONEBOOK_STORE_SLOTUSED					(TBool)(ETrue)
#define DMMTSY_PHONEBOOK_STORE_BATCH_START				2
#define DMMTSY_PHONEBOOK_STORE_BATCH_SIZE				10
#define DMMTSY_PHONEBOOK_TYPE							(KEtelUSimPhoneBookType)
#define DMMTSY_PHONEBOOK_MAXSECONDNAMES					1
#define DMMTSY_PHONEBOOK_MAXTEXTLENSECONDNAME			60
#define DMMTSY_PHONEBOOK_MAXADDNUM						0
#define DMMTSY_PHONEBOOK_MAXTEXTLENADDNUM				80
#define DMMTSY_PHONEBOOK_MAXNUMLENADDNUM				10
#define DMMTSY_PHONEBOOK_MAXGROUPNAMES					2
#define DMMTSY_PHONEBOOK_MAXTEXTLENGROUPNAME			50
#define DMMTSY_PHONEBOOK_MAXEMAILADDR					3
#define DMMTSY_PHONEBOOK_MAXTEXTLENEMAILADDR			40

#define DMMTSY_PHONE_ONE_ENTRY          (1)
#define DMMTSY_PHONE_NUM_OF_ENTRIES     (3)
#define DMMTSY_PHONE_BOOK_DATA1         _L8("01632960000 1 1 Dejana")
#define DMMTSY_PHONE_BOOK_DATA2         _L8("01632960000 1 1 John")
#define DMMTSY_PHONE_BOOK_MULTIPLE_DATA _L8("01632960000 1 1 John,07755121212 2 3 Katie,01486 896 567 0 4 Joan")

#define DMMTSY_CONFERENCE_CAPS							(RMobileConferenceCall::KCapsHangUp | RMobileConferenceCall::KCapsSwap)
#define DMMTSY_CONFERENCE_CALL_NAME						_L("Call1")
#define DMMTSY_CONFERENCE_CALL_COUNT					4
#define DMMTSY_CONFERENCE_CALL_INDEX					1
#define DMMTSY_CONFERENCE_CALL_STATUS					(RMobileConferenceCall::EConferenceActive)
#define DMMTSY_CONFERENCE_CALL_EVENT					(RMobileConferenceCall::EConferenceCallAdded)
#define DMMTSY_CONFERENCE_CALL_LINE						_L("Voice")

// testing auxillary line caps

#define DMMTSY_MOBILE_LINE_AUX_VOICE					(RMobileLine::KCapsAuxVoice)

#define DMMTSY_PHONE_CURRENT_NETWORK_ACCESS_GSM			(RMobilePhone::ENetworkAccessGsm)
#define DMMTSY_PHONE_MULTICALLBEARERMODE_NOTSUPPORTED	(RMobileCall::EMulticallNotSupported)

#define DMMTSY_PHONE_CURRENT_NETWORK_HSDPA_AVAILABILITY (TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_EGPRS_AVAILABILITY	(TBool)(ETrue)
#define DMMTSY_PHONE_CURRENT_NETWORK_HSUPA_AVAILABILITY (TBool)(ETrue)

//testing mailbox numbers
#define DMMTSY_PHONE_MAILBOXNUMBER_VOICE			(31)  
#define DMMTSY_PHONE_MAILBOXNUMBER_DATA				(32)	
#define DMMTSY_PHONE_MAILBOXNUMBER_FAX				(33)  
#define DMMTSY_PHONE_MAILBOXNUMBER_OTHER			(34)
#define DMMTSY_PHONE_MAILBOXNUMBER_VIDEO			(35)


//testing MMS 
#define DMMTSY_PHONE_MMS_UPDATE_SPACE				(TBool)(ETrue)
#define DMMTSY_PHONE_MMS_UPDATE_NOTIFICATION		(TBool)(ETrue)
#define DMMTSY_PHONE_MMS_UPDATE_STATUS				(RMobilePhone::EMmsRetrieved)
#define DMMTSY_PHONE_MMS_UPDATE_IMPLEMENTATION		(RMobilePhone::KWapImplementation)
#define DMMTSY_PHONE_MMS_UPDATE_NOTIFY				(_L8("MMS Notify"))
#define DMMTSY_PHONE_MMS_USER_CONN_PARAMS			(_L8("CONNECTION PARAMETERS"))
#define DMMTSY_PHONE_MMS_USER_PREF_PARAMS			(_L8("PREFERENCE PARAMETERS"))
#define DMMTSY_PHONE_MMS_USER_CONN_PARAMS_TYPE  	(RMobilePhone::EIssuerConnParams)

//Network Investigation 
#define DMMTSY_PHONE_NETWORK_INV_SCAN				(RMobilePhone::KNetworkInvAfterPlmnSelection)
#define DMMTSY_PHONE_NETWORK_INV_SCAN_EVENT			(RMobilePhone::ENetworkInvestigationHighPriorityPlmn)

//APN Control List
#define DMMTSY_PHONE_APN_ENUMERATE					(3)
#define DMMTSY_PHONE_APN_ENTRY1						(_L("GGSN DNS NAME 1"))
#define DMMTSY_PHONE_APN_ENTRY2						(_L("GGSN DNS NAME 2"))
#define DMMTSY_PHONE_APN_ENTRY3						(_L8("GGSN DNS NAME 3"))
#define DMMTSY_PHONE_APN_ENTRY_TO_APPEND				(_L8("GGSN DNS NAME TO APPEND"))
#define DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_ENABLED		(RMobilePhone::EAPNControlListServiceEnabled)
#define DMMTSY_PHONE_APN_CONTROL_LIST_SERVICE_DISABLED		(RMobilePhone::EAPNControlListServiceDisabled)

//testing air time duration

#define DMMTSY_AIR_TIME_DURATION1		350
#define DMMTSY_AIR_TIME_DURATION2		355

//testing starting and stopping of audio tones

#define DMMTSY_AUDIO_START_TONE 		(RMobileCall::EAudioStart)
#define DMMTSY_AUDIO_STOP_TONE  		(RMobileCall::EAudioStop)
#define DMMTSY_AUDIO_CONGESTION_TONE   	(RMobileCall::ECongestionTone)
#define DMMTSY_AUDIO_PLAYED_LOCALLY		(TBool)(ETrue)
#define DMMTSY_AUDIO_NO_TONE (RMobileCall::ENoTone)

#define DMMTSY_PHONE_NOTIFY_SERVICEREQUEST			(RMobilePhone::ESendSSReturnResult)
#define DMMTSY_PHONE_NOTIFY_OPCODE					(0x05)
#define DMMTSY_PHONE_NOTIFY_ADDINFO					(_L("ReturnResult Parameters"))
#define DMMTSY_PHONE_NOTIFY_SERVICEREQUEST_ERROR		(RMobilePhone::ESendSSReturnError)
#define DMMTSY_PHONE_NOTIFY_RETURN_ERROR_PROBLEM_CODE	(0x04)

#define DMMTSY_PHONE_NOTIFY_DTMFEVENT_ON				(RMobilePhone::EStartDtmfTone)

//testing SIM-Personalisation
#define DMMTSY_PHONE_PERS_CAPS_SERV_PROV		(RMobilePhone::KCapsPersonalisedServiceProvider)
#define DMMTSY_PHONE_PERS_CAPS_NETWORK			(RMobilePhone::KCapsPersonalisedNetwork)
#define DMMTSY_PHONE_PERS_GET_NETWORK_SUB		(RMobilePhone::KPersonalisedNetworkSub)
#define DMMTSY_PHONE_PERS_GET_CORPORATE			(RMobilePhone::KPersonalisedCorporate)
#define DMMTSY_PHONE_PERS_SET_USIM				(RMobilePhone::EPersonaliseUsim)
#define DMMTSY_PHONE_PERS_SET_CODE				(_L("1234"))

// testing IMS 

#define DMMTSY_PHONE1_IMPI (_L8("IMPI"))
#define DMMTSY_PHONE1_COUNT (1) 
#define DMMTSY_PHONE1_IMPU0 (_L8("IMPU"))
#define DMMTSY_PHONE1_HNDN (_L8("HNDN"))
#define DMMTSY_PHONE1_DATASOURCE (RMobilePhone::EFromUSIM)


#define DMMTSY_PHONE2_IMPI (_L8("IMPI2"))
#define DMMTSY_PHONE2_COUNT (4) 
#define DMMTSY_PHONE2_IMPU1 (_L8("IMPU2-1"))
#define DMMTSY_PHONE2_IMPU2 (_L8("IMPU2-2"))
#define DMMTSY_PHONE2_IMPU3 (_L8("IMPU2-3"))
#define DMMTSY_PHONE2_IMPU4 (_L8("IMPU2-4"))
#define DMMTSY_PHONE2_HNDN (_L8("HNDN2"))
#define DMMTSY_PHONE2_DATASOURCE (RMobilePhone::EFromISIM)


#define DMMTSY_PHONE3_IMPI (_L8(""))
#define DMMTSY_PHONE3_COUNT (0) 
#define DMMTSY_PHONE3_HNDN (_L8(""))
#define DMMTSY_PHONE3_DATASOURCE (RMobilePhone::EFromUSIM)


#define DMMTSY_PHONE_AUTN_IN (_L8("AUTN_IN"))
#define DMMTSY_PHONE_RAND_IN (_L8("RAND_IN"))
#define DMMTSY_PHONE_RES_IN  (_L8(""))
#define DMMTSY_PHONE_IK_IN  (_L8(""))		
#define DMMTSY_PHONE_CK_IN (_L8(""))
#define DMMTSY_PHONE_AUTS_IN (_L8(""))

#define DMMTSY_PHONE_AUTN_OUT (_L8("0123456789123456"))
#define DMMTSY_PHONE_RAND_OUT (_L8("1234567891234560"))
#define DMMTSY_PHONE_RES_OUT  (_L8("2345678912345601"))
#define DMMTSY_PHONE_IK_OUT  (_L8("345601632960000"))		
#define DMMTSY_PHONE_CK_OUT (_L8("4567891234560123"))
#define DMMTSY_PHONE_AUTS_OUT (_L8("56789123456789"))

// Test values for SmartCardAuthenticate
#define DMMTSY_PHONE_SC_AUTN_IN   (_L8("DMM_SC_AUTH_AUTN"))
#define DMMTSY_PHONE_SC_RAND_IN   (_L8("DMM_SC_AUTH_RAND"))
#define DMMTSY_PHONE_SC_RES_IN    (_L8(""))
#define DMMTSY_PHONE_SC_IK_IN     (_L8(""))
#define DMMTSY_PHONE_SC_CK_IN     (_L8(""))
#define DMMTSY_PHONE_SC_AUTS_IN   (_L8(""))
#define DMMTSY_PHONE_SC_KC_IN     (_L8(""))

#define DMMTSY_PHONE_SC_AUTN_OUT  (_L8("DMM_AUTN_OUT"))
#define DMMTSY_PHONE_SC_RAND_OUT  (_L8("DMM_RAND_OUT"))
#define DMMTSY_PHONE_SC_RES_OUT   (_L8("DMM_RES_OUT"))
#define DMMTSY_PHONE_SC_IK_OUT    (_L8("DMM_IK_OUT"))
#define DMMTSY_PHONE_SC_CK_OUT    (_L8("DMM_CK_OUT"))
#define DMMTSY_PHONE_SC_AUTS_OUT  (_L8("DMM_AUTS_OUT"))
#define DMMTSY_PHONE_SC_KC_OUT    (_L8("DMM_KC_O"))

//_LIT8(DMMTSY_PHONE_GBA_AID, "\x00\x10\x11\x12\x15\x13\x00\x01\x00\x02\x03\x00\x04\x05\x08\x09");
_LIT8(DMMTSY_PHONE_GBA_AID,   "\x00\x01\x01\x02\x03\x00\x04\x05\x08\x09\x00\x10\x11\x12\x11\x12");

#define DMMTSY_PHONE_AUTH_DATA_GBA_PARAMS		(RMobilePhone::EGbaBootstrapParams)
#define DMMTSY_PHONE_AUTH_DATA_NAF_LIST 		(RMobilePhone::EGbaNafIdList)
#define DMMTSY_PHONE_AUTH_DATA_MBMS_MSK			(RMobilePhone::EMbmsMskIdList)
#define DMMTSY_PHONE_AUTH_DATA_MBMS_MUK			(RMobilePhone::EMbmsMuk)

#define DMMTSY_PHONE_GBA_AUTN_IN  (_L8("DMM_GBAAUTH_AUTN"))
#define DMMTSY_PHONE_GBA_RAND_IN  (_L8("DMM_GBAAUTH_RAND"))

#define DMMTSY_PHONE_GBA_AUTN_INV  (_L8("DMM_GBAINV_AUTN"))
#define DMMTSY_PHONE_GBA_RAND_INV  (_L8("DMM_GBAINV_RAND"))

#define DMMTSY_PHONE_GBA_SUCC_RES_OUT  (_L8("DMM_SUC_RES_OUT"))
#define DMMTSY_PHONE_GBA_FAIL_RES_OUT  (_L8(""))

#define DMMTSY_PHONE_GBA_FAIL_AUTS_OUT (_L8("DMM_AUTS_OUT"))
#define DMMTSY_PHONE_GBA_SUCC_AUTS_OUT (_L8(""))

#define DMMTSY_PHONE_GBA_NAFLIST_COUNT		(3)
#define DMMTSY_PHONE_GBA_NAFLIST_BTID_ONE 	(_L8("DMMTSY_PHONE_GBA_NAFLIST_BTID_ONE_EFABCDEFABCDEFABCDEF1234567890123456789012342345678901234567890123456789012345678901234567890123456789012345678567890123456789012345678901234567890190123456789012345678901234567890123456789012345ABCDEFABCDEFABCDEFABCDEAFA"))

#define DMMTSY_PHONE_GBA_NAFLIST_NAFID_ONE	(_L8("DMMTSY_PHONE_GBA_NAFLIST_NAFID_ONE_BCDEFABCDEFABCDEF1234567890123456789012345678901234567890123456780123456789012345678901234567890123456789012345901234567890123456789012345678901234567890123456789012345678967890123456789012345ABCDEFABCDEFABCDEFABCDEFABCD"))

#define DMMTSY_PHONE_GBA_NAFLIST_BTID_TWO	(_L8("DMMTSY_PHONE_GBA_NAFLIST_BTID_TWO_ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234567890123456123456789012345678901234567890123456789012345678901789012345678901234567890123456789012345678901234567890234567890123456789012345678901234567890123"))

#define DMMTSY_PHONE_GBA_NAFLIST_NAFID_TWO	(_L8("DMMTSY_PHONE_GBA_NAFLIST_NAFID_TWO_ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF1234567890123456789012345678901234567890123456789012345678901234567890890123456789012345678901234567890123456789012345678901234567123456789012345678901234567890123456789012"))

#define DMMTSY_PHONE_GBA_NAFLIST_BTID_THREE	(_L8("DMMTSY_PHONE_GBA_NAFLIST_BTID_THREE_ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678903"))

#define DMMTSY_PHONE_GBA_NAFLIST_NAFID_THREE	(_L8("DMMTSY_PHONE_GBA_NAFLIST_BTID_THREE_ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903"))

#define DMMTSY_PHONE_BSF_VALID_BTID		(_L8("DMM_BTID_VALID__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123"))
#define DMMTSY_PHONE_BSF_VALID_KEYLIFETIME	(_L8("DMM_LT_VALID_01234567890123456ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345"))
#define DMMTSY_PHONE_BSF_INVALID_BTID		(_L8("DMM_BTID_INV"))
#define DMMTSY_PHONE_BSF_INVALID_KEYLIFETIME	(_L8("DMM_LT_INV"))

#define DMMTSY_PHONE_GBA_VALID_NAFID		(_L8("DMM_VAL_NAF__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123432"))
#define DMMTSY_PHONE_GBA_VALID_IMPI		(_L8("DMM_VAL_IMPI__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF1234567890123451234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890678901234567890123456789012345678901234567890123456789030123456789012345612316"))
#define DMMTSY_PHONE_GBA_INVALID_NAFID		(_L8("DMM_INV_NAFID"))
#define DMMTSY_PHONE_GBA_INVALID_IMPI		(_L8("DMM_INV_IMPI"))

#define DMMTSY_PHONE_GBA_KS_EXTNAF		(_L8("DMM_EXT_NAF__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123136"))

_LIT8(DMMTSY_PHONE_MBMS_AID, "\x00\x10\x11\x12\x00\x04\x05\x08\x15\x13\x00\x01\x00\x02\x03\x09");
#define DMMTSY_PHONE_MBMS_VALID_IMIKEY_WO_VBIT	(_L8("DMM_IMIK_WO_V"))
#define DMMTSY_PHONE_MBMS_VALID_IMIKEY_W_VBIT	(_L8("DMM_IMIK_W_V"))
#define DMMTSY_PHONE_MBMS_VALID_IMIKEY		(_L8("DMM_IMIK_VAL"))
#define DMMTSY_PHONE_MBMS_INVALID_IMIKEY	(_L8("DMM_IMIK_INV"))

#define DMMTSY_PHONE_MBMS_OMIKEY_FOR_W_VBIT	(_L8("DMM_OMIK_W_V"))
#define DMMTSY_PHONE_MBMS_VALID_MTK		(_L8("DMM_VAL_MTK__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123136"))
#define DMMTSY_PHONE_MBMS_VALID_SALTKEY		(_L8("DMM_VAL_SKEY"))

#define DMMTSY_PHONE_MBMS_VALID_KEYDMN		(_L8("KDN"))
#define DMMTSY_PHONE_MBMS_VALID_MSKIDGRP	(_L8("GR"))
#define DMMTSY_PHONE_MBMS_INVALID_KEYDMN	(_L8("IKN"))
#define DMMTSY_PHONE_MBMS_INVALID_MSKIDGRP	(_L8("IG"))

#define DMMTSY_PHONE_MBMS_VALID_MUKID		(_L8("DMM_VAL_MUKID__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678901234561231"))
#define DMMTSY_PHONE_MBMS_INVALID_MUKID		(_L8("DMM_INV_MUKID"))
#define DMMTSY_PHONE_MBMS_VALID_MUK_IDI		(_L8("DMM_VAL_MUK_IDI__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF1234567890123451234567890123456789012345678901234567890123456789012345678923456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678901234561231"))
#define DMMTSY_PHONE_MBMS_VALID_MUK_IDR		(_L8("DMM_VAL_MUK_IDR__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF1234567890123451234567890123456789012345678901234567890123456789012345678923456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678901234561231"))
#define DMMTSY_PHONE_MBMS_VALID_MUK_ITIMESTM	(_L8("TSTM"))

#define DMMTSY_PHONE_MBMS_LIST_COUNT		(3)

#define DMMTSY_PHONE_MBMSLIST_KEYDMN_ONE 	(_L8("KY1"))
#define DMMTSY_PHONE_MBMSLIST_MSKID_ONE	    	(_L8("MSK1"))
#define DMMTSY_PHONE_MBMSLIST_TIMESTM_ONE	(_L8("STM1"))

#define DMMTSY_PHONE_MBMSLIST_KEYDMN_TWO 	(_L8("KY2"))
#define DMMTSY_PHONE_MBMSLIST_MSKID_TWO	  	(_L8("MSK2"))
#define DMMTSY_PHONE_MBMSLIST_TIMESTM_TWO	(_L8("STM2"))

#define DMMTSY_PHONE_MBMSLIST_KEYDMN_THREE	(_L8("KY3"))
#define DMMTSY_PHONE_MBMSLIST_MSKID_THREE	(_L8("MSK3"))
#define DMMTSY_PHONE_MBMSLIST_TIMESTM_THREE	(_L8("STM3"))

#define DMMTSY_PHONE_WLAN_SIDLIST_COUNT		(3)

#define DMMTSY_PHONE_WLAN_PSEUDONYM			(RMobilePhone::KWlanPseudonym)
#define DMMTSY_PHONE_WLAN_REAUTHID			(RMobilePhone::KWlanReauthenticationId)
#define DMMTSY_PHONE_WLAN_MASTERKEY			(RMobilePhone::KWlanMasterKey)
#define DMMTSY_PHONE_WLAN_COUNTER			(RMobilePhone::KWlanCounter)
#define DMMTSY_PHONE_WLAN_LIST_DATA_FLAGS	((DMMTSY_PHONE_WLAN_PSEUDONYM | \
											  DMMTSY_PHONE_WLAN_REAUTHID |\
											  DMMTSY_PHONE_WLAN_MASTERKEY |\
											  DMMTSY_PHONE_WLAN_COUNTER ))

#define DMMTSY_PHONE_WLAN_VALID_PSEUDONYM	(_L8("DMM_VAL_PNYM__ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345690301234567890123456123432"))			
#define DMMTSY_PHONE_WLAN_VALID_REAUTHID	(_L8("DMM_VAL_REAID_ABCD78ABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678908712345678901234523435"))		
#define DMMTSY_PHONE_WLAN_VALID_MASTERKEY	(_L8("DMM_VAL_MASTERKEY_EFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678123456123435"))	
#define DMMTSY_PHONE_WLAN_VALID_COUNTER		(_L8("DMM_VAL_COUNTER_CDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF123456789012345123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789067890123456789012345678901234567890123456789012345678903012345678923456123435"))		

#define DMMTSY_PHONE_WLAN_INVALID_PSEUDONYM	(_L8("DMM_INV_PNYM"))			
#define DMMTSY_PHONE_WLAN_INVALID_REAUTHID	(_L8("DMM_INV_REAID"))		
#define DMMTSY_PHONE_WLAN_INVALID_MASTERKEY	(_L8("DMM_INV_MASTERKEY"))
#define DMMTSY_PHONE_WLAN_INVALID_COUNTER	(_L8("DMM_INV_CTR"))		

#define DMMTSY_PHONE_WLAN_WSID_ONE			(_L8("DMM_SID_1_CDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123435ACDB"))
#define DMMTSY_PHONE_WLAN_USERDEFD_ONE		(TBool)(ETrue)

#define DMMTSY_PHONE_WLAN_WSID_TWO			(_L8("DMM_SID_2_CDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF12345678901234512345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678906789012345678901234567890123456789012345678901234567890301234567890123456123435ACDB"))
#define DMMTSY_PHONE_WLAN_USERDEFD_TWO		(TBool)(ETrue)

#define DMMTSY_PHONE_WLAN_WSID_THREE		(_L8("DMM_SID_3_CDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEFABCDEF1234567890123451234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890678901234567890123456789012345678901234567890123456789030123456789012345612343BED56"))
#define DMMTSY_PHONE_WLAN_USERDEFD_THREE	(TBool)(EFalse)

#define DMMTSY_PHONE_WLAN_INVALID_WSID_ONE		(_L8("DMM_INV_SID_1"))
#define DMMTSY_PHONE_WLAN_INVALID_USERDEFD_ONE 	(TBool)(ETrue)

#define DMMTSY_PHONE_WLAN_INVALID_WSID_TWO		(_L8("DMM_INV_SID_2"))
#define DMMTSY_PHONE_WLAN_INVALID_USERDEFD_TWO 	(TBool)(EFalse)

#define DMMTSY_PHONE_WLAN_INVALID_WSID_THREE		(_L8("DMM_INV_SID_3"))
#define DMMTSY_PHONE_WLAN_INVALID_USERDEFD_THREE 	(TBool)(EFalse)

// Testing Smart Card EAP
_LIT8(DMMTSY_PHONE_EAPAPP_AID, "\x00\x01\x00\x02\x03\x00\x04\x05\x08\x09\x00\x10\x11\x12\x15\x16");
#define DMMTSY_PHONE_EAP_METHOD    KETelSmartCardEapTypeAKA

#define DMMTSY_PHONE1_EAP_IDTYPE   (RMobileSmartCardEap::EPermanentIdentity)
#define DMMTSY_PHONE2_EAP_IDTYPE   (RMobileSmartCardEap::EPseudonymIdentity)
#define DMMTSY_PHONE_EAPAUTHSTATUS (RMobileSmartCardEap::EAuthenticating)
#define DMMTSY_PHONE1_EAP_UID      (_L8("DMM_Eap_SC_APP_UID_1"))
#define DMMTSY_PHONE2_EAP_UID      (_L8("DMM_Eap_SC_APP_UID_2"))
#define DMMTSY_PHONE1_EAPPACKET    (_L8("EAP_SC_App_AuthPacket_1244070100000001eap1.foo"))
#define DMMTSY_PHONE2_EAPPACKET    (_L8("EAP_SC_App_AuthPacket_1244070100000001eap2.foo"))
#define DMMTSY_PHONE1_EAPPACKET2   (_L8("EAP_SC_App_AuthPacket2_1244070100000001eap1123456789012345678901234567890123456789012345678901234567890.foo"))
#define DMMTSY_PHONE2_EAPPACKET2   (_L8("EAP_SC_App_AuthPacket2_1244070100000001eap2123456789012345678901234567890123456789012345678901234567890.foo"))
#define DMMTSY_PHONE1_EAPKEY       (_L8("EAP_SC_App_Key1_000111222333444555666777888999.foo"))
#define DMMTSY_PHONE2_EAPKEY       (_L8("EAP_SC_App_Key2_000111222333444555666777888999.foo"))
#define DMMTSY_PHONE1_EAPKEYTAG    (RMobileSmartCardEap::EEapKeyEMSK)
#define DMMTSY_PHONE2_EAPKEYTAG    (RMobileSmartCardEap::EEapKeyMSK)


//Testing Location Services Requests
//Testing MTLR Requests
//Added

#define DMMTSY_SETLCS_DOMAIN1    (RMobileLocationServices::EPacketSwitchedDomain)
#define DMMTSY_SETLCS_DOMAIN2    (RMobileLocationServices::ECircuitSwitchedDomain)

#define DMMTSY_LOCATION_ESTIMATE_TYPE1    (RMobileLocationServices::ECurrentLocation)
#define DMMTSY_LOCATION_ESTIMATE_TYPE2    (RMobileLocationServices::ECurrentOrLastKnownLocation)

#define DMMTSY_NOTIFICATION_TYPE1         (RMobileLocationServices::ENotifyLocationAllowed)
#define DMMTSY_NOTIFICATION_TYPE2         (RMobileLocationServices::ENotifyAndVerify_LocationAllowedIfNoResponse)
#define DMMTSY_NOTIFICATION_TYPE3         (RMobileLocationServices::ENotifyAndVerify_LocationNotAllowedIfNoResponse)
#define DMMTSY_NOTIFICATION_TYPE4         (RMobileLocationServices::ELocationPrivacyOverride)
#define DMMTSY_NOTIFICATION_TYPE5         (RMobileLocationServices::ELocationNotAllowed )

#define DMMTSY_DATA_CODING_SCHEME		(TUint8)(0)
#define DMMTSY_DATA_CODING_SCHEME1		(TUint8)(1)
#define DMMTSY_DATA_CODING_SCHEME2		(TUint8)(2)
#define DMMTSY_DATA_CODING_SCHEME3		(TUint8)(3)
#define DMMTSY_DATA_CODING_SCHEME4		(TUint8)(4)
#define DMMTSY_DATA_CODING_SCHEME5		(TUint8)(5)
#define DMMTSY_DATA_CODING_SCHEME6		(TUint8)(6)

#define DMMTSY_MTLR_RESPONSE1              (RMobileLocationServices::EPermissionGranted)
#define DMMTSY_MTLR_RESPONSE2              (RMobileLocationServices::EPermissionDenied)

#define DMMTSY_MTLR_ERROR1                 (RMobileLocationServices::ESystemFailure)
#define DMMTSY_MTLR_ERROR2                 (RMobileLocationServices::EUnexpectedDataValue)

#define DMMTSY_MTLR_REJECT1                (RMobileLocationServices::EDuplicateInvokeID)
#define DMMTSY_MTLR_REJECT2                (RMobileLocationServices::EUnrecognizedOperation)
#define DMMTSY_MTLR_REJECT3                (RMobileLocationServices::EResourceLimitation)
#define DMMTSY_MTLR_REJECT4                (RMobileLocationServices::EInitiatingRelease)
#define DMMTSY_MTLR_REJECT5                (RMobileLocationServices::EUnrecognizedLinkedID)
#define DMMTSY_MTLR_REJECT6                (RMobileLocationServices::ELinkedResponseUnexpected)
#define DMMTSY_MTLR_REJECT7                (RMobileLocationServices::EUnexpectedLinkedOperation)
#define DMMTSY_MTLR_REJECT8                (RMobileLocationServices::EMistypedParameter)

#define DMMTSY_EXTERNAL_CLIENT_NAME       _L8("xyz")
#define DMMTSY_EXTERNAL_CLIENT_ID         _L8("00ABC1234TH")
#define DMMTSY_INVALID_EXTERNAL_CLIENT_ID         _L8(" ")

#define DMMTSY_MOLR_TYPE1                  (RMobileLocationServices::ELocationEstimate)
#define DMMTSY_MOLR_TYPE2                  (RMobileLocationServices::EAssistanceData)
#define DMMTSY_MOLR_TYPE3                  (RMobileLocationServices::EDecipheringKeys)

#define DMMTSY_MOLR_LOCATION_ESTIMATE                  _L8("ABC1245.123")
#define DMMTSY_MOLR_VELOCITY_ESTIMATE                  _L8("ABC1245.123")

#define DMMTSY_MOLR_GPS_ASSISTANCE_DATA		_L8("ABC")
		
#define DMMTSY_MOLR_RESPONSE_TIME1 		(RMobileLocationServices::ELowDelay)
#define DMMTSY_MOLR_RESPONSE_TIME2 		(RMobileLocationServices::EDelayTolerant)

#define DMMTSY_MOLR_LOCATION_METHOD1 		(RMobileLocationServices::EMSBasedEOTD)
#define DMMTSY_MOLR_LOCATION_METHOD2 		(RMobileLocationServices::EMSAssistedEOTD)
#define DMMTSY_MOLR_LOCATION_METHOD3 		(RMobileLocationServices::EMSAssistedGPS)
#define DMMTSY_MOLR_LOCATION_METHOD4 		(RMobileLocationServices::EMSBasedOTDOA)

#define DMMTSY_MOLR_HORIZONTAL_ACCURACY		(TUint8)(2222222)
#define DMMTSY_MOLR_VERTICAL_ACCURACY		(TUint8)(1111111)

#define DMMTSY_MOLR_LOCATION_ESTIMATION_TYPE1 		(RMobileLocationServices::ECurrentLocation)
#define DMMTSY_MOLR_LOCATION_ESTIMATION_TYPE2 		(RMobileLocationServices::ECurrentOrLastKnownLocation)
#define DMMTSY_MOLR_LOCATION_ESTIMATION_TYPE3 		(RMobileLocationServices::EInitialLocation)

#define DMMTSY_MOLR_ERROR_TYPE1                  (RMobileLocationServices::EMolrSystemFailure)
#define DMMTSY_MOLR_ERROR_TYPE2                  (RMobileLocationServices::EMolrUnexpectedDataValue)
#define DMMTSY_MOLR_ERROR_TYPE3                  (RMobileLocationServices::EMolrDataMissing)
#define DMMTSY_MOLR_ERROR_TYPE4                  (RMobileLocationServices::EMolrFacilityNotSupported)
#define DMMTSY_MOLR_ERROR_TYPE5                  (RMobileLocationServices::EMolrSS_SubscriptionViolation)
#define DMMTSY_MOLR_ERROR_TYPE6                  (RMobileLocationServices::EMolrPositionMethodFailure)

//A-GPS Macros
#define DMMTSY_PHONE_LCS_MEASUREMENTID		(TUint8)(100)
#define DMMTSY_PHONE_LCS_MEASUREMENTID1		(TUint8)(1)
#define DMMTSY_PHONE_LCS_INVALID_MEASUREMENTID		(TUint8)(0)
#define DMMTSY_PHONE_LCS_MODIFIED_MEASUREMENTID (TUint8)(200)

#define DMMTSY_PHONE_LCS_MCTRLFAILURE1		(RMobileLocationServices::EAsn1ViolationOrEncoding)
#define DMMTSY_PHONE_LCS_MCTRLFAILURE2		(RMobileLocationServices::EMsgTypeNonExistent)
#define DMMTSY_PHONE_LCS_MCTRLFAILURE3		(RMobileLocationServices::EMsgNotCompatibleWithReceiverState)
#define DMMTSY_PHONE_LCS_MCTRLFAILURE4		(RMobileLocationServices::EIeValueNotComprehend)
#define DMMTSY_PHONE_LCS_MCTRLFAILURE5		(RMobileLocationServices::EInformationElementMissing)
#define DMMTSY_PHONE_LCS_MCTRLFAILURE6		(RMobileLocationServices::EMsgExtnNotComprehend)

#define DMMTSY_PHONE_LCS_MC_POSTYPE			(RMobileLocationServices::EUeBased)
#define DMMTSY_PHONE_LCS_MC_POSMETHOD		(RMobileLocationServices::EGps)
#define DMMTSY_PHONE_LCS_MC_CHANNELID		(TUint8)(10)
#define DMMTSY_PHONE_LCS_MC_TOTALCRC		(TUint)(1)
#define DMMTSY_PHONE_LCS_MC_BADCRC			(TUint)(0)
#define DMMTSY_PHONE_LCS_MC_PENDING			(TUint)(0)
#define DMMTSY_PHONE_LCS_MC_NOREPORTING 	(TBool)(ETrue)		
#define DMMTSY_PHONE_LCS_MC_HORZACCURACY 	(TInt8)(0)		
#define DMMTSY_PHONE_LCS_MC_VERTACCURACY 	(TInt8)(0)			
#define DMMTSY_PHONE_LCS_MC_GPSTIMEOFCELLWANTED 	(TBool)(EFalse)		
#define DMMTSY_PHONE_LCS_MC_ADDLASSISTANCEDATAREQ 	(TBool)(ETrue)		
	
#define DMMTSY_PHONE_LCS_MC_VERIFIEDBSIC 	(TUint8)(10)		
#define DMMTSY_PHONE_LCS_MC_NONVERIFIEDBSIC (TUint8)(0)			
#define DMMTSY_PHONE_LCS_MC_PRIMARYSCRAMBLECODE (TUint)(1)		
#define DMMTSY_PHONE_LCS_MC_CELLPARAMSID	(TUint)(50)	
#define DMMTSY_PHONE_LCS_MC_CELLID 			(TUint)(100)		
#define DMMTSY_PHONE_LCS_MC_TRAFFICVOLMEASRES	(TUint8)(5) 		
#define DMMTSY_PHONE_LCS_MC_QLTYMEASRES 	(TBool)(ETrue)
#define DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE (RMobileLocationServices::EAckModeRlc)
#define DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK	TUint(1)
#define DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE TUint(1)

#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1		(RMobileLocationServices::ENorth)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN2		(RMobileLocationServices::ESouth)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LAT		TUint(100)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LONG	TUint(200)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_DIRECTION1	TUint(EHeight)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_DIRECTION2	TUint(EDepth)
#define DMMTSY_PHONE_LCS_MC_GPS_REFLOC_ALT			TUint(1000)
#define DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW		TUint(100.10)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID	TUint8(122)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1	(RMobileLocationServices::ENsNnU)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS2	(RMobileLocationServices::EEsSn)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS3	(RMobileLocationServices::EEsNnU)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS4	(RMobileLocationServices::ERev2)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS5	(RMobileLocationServices::ERev)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS	TUint8(99)
#define DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATHEALTH	TUint(100.100)
#define DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1		TUint8(111)
#define DMMTSY_PHONE_LCS_MC_GPS_UTC_MODEL1				TUint8(222)
#define DMMTSY_PHONE_LCS_MC_GPS_ALMANAC					TUint8(111)
#define DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME		TUint32(999999090)
#define DMMTSY_PHONE_LCS_MC_GPS_MODE_PRIMARYCODE		TUint16(1)
#define DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID		TUint16(1)
#define DMMTSY_PHONE_LCS_MC_GPS_BADSATLIST				TUint8(0)

#define DMMTSY_PHONE_LCS_MC_VELOCITY_REQUESTED	(TBool)(ETrue)
#define DMMTSY_PHONE_LCS_MC_VELOCITY_NOTREQUESTED		(TBool)(EFalse)
#define DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_REFTIMEST_REQUESTED		(TBool)(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_REFTIMEST_NOTREQUESTED		(TBool)(ETrue)

//measurement report parameters
//Velocity estimate macros

#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS1		(RMobileLocationServices::EHorzVelPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS2		(RMobileLocationServices::EHorzWithVertVelPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS3		(RMobileLocationServices::EHorzVelWithUncertPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS4		(RMobileLocationServices::EHorzWithVertVelAndUncertPresent)

// if DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS1
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_BEARING	TUint16(11111)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZ_SPEED		TUint16(111.0001)
// if DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS2
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHVERT_BEARING	TUint16(10000)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHVERT_SPEED		TUint16(100.0001)
// if DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS3
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHUNCERTAINITY_BEARING	TUint16(10001)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHUNCERTAINITY_SPEED		TUint16(101.0001)
// if DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_STATUS4
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHVERTANDUNCERTAINITY_BEARING	TUint16(100111)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHVERTANDUNCERTAINITY_HORZSPEED		TUint16(01111.0001)
#define DMMTSY_PHONE_LCS_MC_GPS_VELOCITYEST_HORZWITHVERTANDUNCERTAINITY_VERTSPEED		TUint16(0111.0001)

// Position Estimate ReferenceInfo
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO1		(RMobileLocationServices::EUtranGpsRefTimeResultPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO2		(RMobileLocationServices::EGpsRefTimeOnlyPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO3		(RMobileLocationServices::ECellTimingPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO4		(RMobileLocationServices::EPosEstimateRefTimePresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO5		(RMobileLocationServices::EEllipsoidPointPresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO6		(RMobileLocationServices::EEllipsoidPointUncertCirclePresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO7		(RMobileLocationServices::EEllipsoidPointUncertEllipsePresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO8		(RMobileLocationServices::EEllipsoidPointAltitudePresent)
#define DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO9		(RMobileLocationServices::EEllipsoidPointAltEllipsePresent)

//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO1
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MODESTATUS_REQUESTED		(TBool)(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANMODE_MODE_FDDID		TUint16(232323)
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANMODE_MODE_TDDID		TUint8(123232)
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART	TUint16(1)
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART	TUint16(1)
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN	TUint16(1)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO2
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_ONLY	TUint16(12.00.10)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO3
#define DMMTSY_PHONE_LCS_MC_GPS_UTRANREFCELLTIME_SFN	TUint16(10.00.10)

//Position estimate macros
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPT_LATSIGN1  	(RMobileLocationServices::ENorth)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPT_LATSIGN2  	(RMobileLocationServices::ENorth)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPTALTELIPSE_DIRECTION1  	(RMobileLocationServices::EHeight)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSOIDPTALTELIPSE_DIRECTION2  	(RMobileLocationServices::EDepth)

//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO5
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LAT	TUint16(100)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPT_LON	TUint16(200)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO6
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNCIRCLE_LAT	TUint16(100)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNCIRCLE_LON	TUint16(200)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNCIRCLE_UNCRTNCODE	TUint16(200)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO7
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNELIPSE_LON	TUint16(200)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNELIPSE_LAT	TUint16(100)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNELIPSE_UNCRTNMAJOR	TUint8(300)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTUNCRTNELIPSE_UNCRTNMINOR	TUint8(150)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO8
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALT_LON	TUint16(200)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALT_LAT	TUint16(100)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALT_ALT 	TUint16(200)
//if DMMTSY_PHONE_LCS_MC_GPS_POSESTINFO9
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALTELIPSE_LAT	TUint16(100)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALTELIPSE_LON	TUint16(200)
#define DMMTSY_PHONE_LCS_MC_GPS_POSEST_ELIPSPTPTALTELIPSE_ALTDIR	TUint16(200)

//Measurement ParamList
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR1 	(RMobileLocationServices::ENm)
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR2 	(RMobileLocationServices::ELow)
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR3 	(RMobileLocationServices::EMedium)
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_PATHINDICATOR4 	(RMobileLocationServices::EHigh)
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_GPSCHIPS			TUint16(3333444)
#define DMMTSY_PHONE_LCS_MC_GPS_PARAMLIST_SATID				TUint8(23423)

#define DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_FALSE	TBool(EFalse)
#define DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_TRUE	TBool(ETrue)

//Measurement control Error Cause
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE1		(RMobileLocationServices::ENotEnoughGpsSatellites)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE2		(RMobileLocationServices::EAssistanceDataMissing)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE3		(RMobileLocationServices::ENotAccomplishedGpsTimingOfCellFrames)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE4		(RMobileLocationServices::EUndefinedError)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE5		(RMobileLocationServices::ERequestDeniedByUser)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE6		(RMobileLocationServices::ENotProcessedAndTimeout)
#define DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE7		(RMobileLocationServices::EReferenceCellNotServingCell)

#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_ALMANACREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_UTCREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_IONREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_NAVREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_DPSREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REFTIMEREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REFLOCREQ		TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_ACQASSITREQ	TBool(ETrue)
#define DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REALTIMEINTGREQ	TBool(ETrue)

//UE Positioning Technology reset notifications
#define DMMTSY_PHONE_LCS_UE_POS_TECH			(RMobileLocationServices::KUePosTechnologyAGPS)

#endif // __TESTDEF_H

