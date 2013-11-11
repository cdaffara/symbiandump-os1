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
// Testing name definition
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __TESTDEF_H
#define __TESTDEF_H

#include "Etelsat.h"

//**********************************************************************************
// General Testing name definitions
//**********************************************************************************

const TInt DSATTSY_TIMEOUT = 100000;

#define DSATTSY_PHONE_NAME				_L("DMobile")
#define DSATTSY_MODULE_NAME				_L("DSAT")

// CoreTsy functionality
#define DSATTSY_CALL_BEARER_CAPS		RCall::KBearerCapsProtocolALT
#define DSATTSY_CALL_BEARER_SPEED		RCall::EBearerData4800

#define DSATTSY_FAX_BUF_SIZE			(50)
typedef TBuf8<DSATTSY_FAX_BUF_SIZE>		TAcqFaxBuf;

#define DSATTSY_NETWORK_TYPE			(RTelServer::ENetworkTypeMobileDigital)
#define DSATTSY_NUMBER_OF_PHONES		(1)
#define DSATTSY_NUMBER_OF_LINES			(4)

#define DSATTSY_CORE_CALL_STATUS		RCall::EStatusIdle
#define DSATTSY_CALL_DURATION1			(10)
#define DSATTSY_CORE_LINE_CAPS			(RLine::KCapsVoice)
#define DSATTSY_CORE_LINE_STATUS		RCall::EStatusIdle
#define DSATTSY_CORE_LINE_STATUS2		RCall::EStatusRinging
#define DSATTSY_CORE_PHONE_STATUS		RPhone::EDetectedPresent
#define DSATTSY_GET_PROGRESS_SPEED		(11)
#define DSATTSY_MYFAX					_L("MY FAX") 

#define DSATTSY_SAT_NAME				_L("TOOLKIT")

//**********************************************************************************
// Testing RSat
//**********************************************************************************

#define DSATTSY_DISPLAY_TEXT_NAME		RSat::EDisplayText
#define DSATTSY_TEXT_TO_DISPLAY			_L("TextToDisplay")
#define DSATTSY_TEXT_ATTRIBUTE_STATUS	RSat::ETextAttributeProvided
#define DSATTSY_TEXT_ATTRIBUTE_STATUS2	RSat::ETextAttributeProvided
#define DSATTSY_TEXT_ATTRIBUTE_STATUS3	RSat::ETextAttributeProvided
#define DSATTSY_TEXT_ATTRIBUTE_STATUS4	RSat::ETextAttributeProvided

#define DSATTSY_TEXT_ATTRIBUTE_DATA		{0x01,0x02,0x03,0x04}
#define DSATTSY_TEXT_ATTRIBUTE_DATA2	{0x05,0x06,0x07,0x08}
#define DSATTSY_TEXT_ATTRIBUTE_DATA3	{0x09,0x10,0x11,0x12}
#define DSATTSY_TEXT_ATTRIBUTE_DATA4	{0x09,0x10,0x11,0x12}
#define DSATTSY_TEXT_ATTRIBUTE_DATA_LENGTH (4)
#define DSATTSY_TEXT_PRIORITY			RSat::EHighPriority
#define DSATTSY_CLEAR_SCREEN_TRIGGER	RSat::EClearAfterDelay
#define DSATTSY_IMMEDIATE_RSP			RSat::ENoImmediateRsp
#define DSATTSY_TEXT_ENCODING_SCHEME	RSat::E16bitUCS2
#define DSATTSY_TEXT_ENCODING_SCHEME_8	RSat::E7bitUnpacked
// The length of the following string must be 276 Unicode characters (maximum supported by RSat)
#define DSATTSY_IDLE_MODE_TEXT			_L("This is a very long string, provided to test that ETelSat handles long Unicode strings correctly. The maximum supported length is 276 Unicode UCS-2 characters. The last character of this text is a 'greater than' symbol. TESTING TESTING TESTING TESTING TESTING TESTING TESTING>")
#define DSATTSY_IDLE_MODE_TEXT_8BIT		_L8("This is a very long string8 provided to test that ETelSat handles long Unicode strings correctly. The maximum supported length is 276 Unicode UCS-2 characters. The last character of this text is a 'greater than' symbol. TESTING TESTING TESTING TESTING TESTING TESTING TESTING>")

#define DSATTSY_ICON_ID1				(1) 
#define DSATTSY_ICON_QUALIFIER1			RSat::ESelfExplanatory
#define DSATTSY_ICON_ID2				(2) 
#define DSATTSY_ICON_QUALIFIER2			RSat::ENotSelfExplanatory

#define DSATTSY_USER_TEXT_INPUT			_L("1")
#define DSATTSY_NETWORK_TEXT_STRING		_L("Network Text String")
#define DSATTSY_USER_RSP_FORMAT				RSat::EDigitOnly
#define DSATTSY_ID1						(0x01)
#define DSATTSY_ID2						(0x02)
#define DSATTSY_ID3						(0x03)

#define DSATTSY_HELP					RSat::ENoHelpAvailable

#define DSATTSY_INPUT_DISPLAY_OPTION	RSat::EDisplayUserInput
#define DSATTSY_GET_INPUT_RSP_FORMAT	RSat::EDigitOnlyPacked
#define DSATTSY_MIN_RSP_LENGTH			(1)
#define DSATTSY_MAX_RSP_LENGTH			(10)
#define DSATTSY_DEFAULT_TEXT			_L("DefaultText")

#define DSATTSY_ALPHA_ID1				_L("AlphaId1")
#define DSATTSY_ALPHA_ID1_STATUS		RSat::EAlphaIdProvided
#define DSATTSY_ALPHA_ID2				_L("AlphaId2")
#define DSATTSY_ALPHA_ID2_STATUS		RSat::EAlphaIdProvided

#define DSATTSY_TONE					RSat::KGeneralBeep
#define DSATTSY_TIME_UNIT				RSat::ESeconds
#define DSATTSY_NUM_OF_UNITS			(2)

#define DSATTSY_SMS_TPDU_FORMAT			RSat::EFormatGsmTpduNoScPrefix
#define DSATTSY_SMS_TPDU				_L8("TPDU")
#define DSATTSY_SMS_REF					(1234)

#define DSATTSY_SS_STRING				_L8("SsString")

#define DSATTSY_DCS						(1) 
#define DSATTSY_USSD_STRING				_L("UssdString")

#define DSATTSY_SET_UP_CALL_TYPE		RSat::EOnlyIfNotBusy

#define DSATTSY_REFRESH_TYPE			RSat::EFileChangeNotification
#define DSATTSY_REFRESH_ADN_EF			RSat::KAdnEf
#define DSATTSY_REFRESH_FDN_EF			RSat::KFdnEf


#define DSATTSY_SET_UP_EVENT_LIST_TYPE	RSat::EUpdateEventList
#define DSATTSY_EVENTS					(0x30) 

#define DSATTSY_IDLE_MODE_TXT_TYPE		RSat::EUpdateIdleModeText

#define DSATTSY_DTMF_STRING				_L8("DTMF String") 

#define DSATTSY_CCP1					_L8("CCP1")
#define DSATTSY_CCP2					_L8("CCP2")
#define DSATTSY_SUBADDRESS				_L("012345678")
#define DSATTSY_TON						RSat::ENationalNumber
#define DSATTSY_NPI						RSat::EIsdnNumberPlan
#define DSATTSY_TEL_NUMBER				_L("01632960000")
#define DSATTSY_CC_RESULT				RSat::EAllowedWithModifications
#define DSATTSY_MODIFIED_TEL_NUMBER		_L("01632960000")
#define DSATTSY_NO_ALPHAID				RSat::ENoAlphaId
#define DSATTSY_NULL_BUF				_L("")
#define DSATTSY_ALPHAID_STATUS			RSat::EAlphaIdNotPresent
#define DSATTSY_BC_REPEAT_INDICATOR_ALTERNATE (0)
#define DSATTSY_BC_REPEAT_INDICATOR_SEQUENTIAL (1)
#define DSATTSY_CC_CALL_PARAM_ORIGIN	RSat::EOriginatorEtelClient

#define DSATTSY_BC_REPEAT_INDICATOR_FALLBACK (RSat::EBCFallbackMode)
#define DSATTSY_BC_REPEAT_INDICATOR_SERVICECHANGEANDFALLBACK (RSat::EBCServiceChangeAndFallbackMode)
#define DSATTSY_PDP_PARAMETERS _L8("01234567890123456789")
#define DSATTSY_PDP_PARAMETERS2 _L8("01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\
01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901")

#define	DSATTSY_CALL_NAME				_L("Caller")
#define	DSATTSY_ACTION_ORIGINATOR		RSat::EClientOriginator

#define DSATTSY_ICON_EF1				_L8("IconEf1")
#define DSATTSY_ICON_EF2				_L8("IconEf2")
#define DSATTSY_IMG_INSTANCE1			_L8("ImgInstance1")
#define DSATTSY_IMG_INSTANCE2			_L8("ImgInstance2")
#define DSATTSY_IMG_INSTANCE_UNKNOWN	_L8("Unknown Instance")
#define DSATTSY_INSTANCE_NUMBER1		(1)
#define DSATTSY_INSTANCE_NUMBER2		(2)
#define DSATTSY_INSTANCE_OFFSET1		(12)
#define DSATTSY_INSTANCE_LENGTH1		(32)
#define DSATTSY_INSTANCE_OFFSET2		(45)
#define DSATTSY_INSTANCE_LENGTH2		(16)
#define DSATTSY_INSTANCE_CODING_BASIC	RSat::KBasic
#define DSATTSY_INSTANCE_CODING_COLOUR	RSat::KColour
#define DSATTSY_INSTANCE_WIDTH_1		(10)
#define DSATTSY_INSTANCE_WIDTH_2		(20)
#define DSATTSY_INSTANCE_HEIGHT_1		(11)
#define DSATTSY_INSTANCE_HEIGHT_2		(21)
#define DSATTSY_INSTANCE_FILE_ID_1		(12)
#define DSATTSY_INSTANCE_FILE_ID_2		(22)


#define DSATTSY_CLUT_OFFSET				(20)
#define DSATTSY_CLUT1					_L8("Clut1")
#define DSATTSY_CLUT2					_L8("Clut2")

#define DSATTSY_HELP_REQUESTED			RSat::EHelpRequested

#define DSATTSY_ITEM_ID					(1)
#define DSATTSY_RSP_ITEM_ID				_L("1")

#define DSATTSY_ITEM1					_L("Item1")
#define DSATTSY_ITEM2					_L("Item2")
#define DSATTSY_ITEM3					_L("Item3")
#define DSATTSY_ITEM4					_L("Item4")

#define DSATTSY_CONTROL_RESULT			RSat::EAllowedNoModification

#define DSATTSY_PRESENTATION_TYPE		RSat::EDataValues
#define DSATTSY_DEFAULT_ITEM			(0)

#define DSATTSY_PCMD_RESULT_SUCCESS		RSat::KSuccess
#define DSATTSY_PCMD_RSP_NO_INFO		RSat::KNoAdditionalInfo
#define DSATTSY_PCMD_RSP_TEXT_STRING	RSat::KTextString
#define DSATTSY_PCMD_RSP_ITEM_ID		RSat::KItemIdentifier

#define DSATTSY_ITEMS					RSat::KItems
#define DSATTSY_ITEMS_AND_ACTIONS		(RSat::KItems|RSat::KItemNextActions)
#define DSATTSY_ITEMS_AND_ICONS			(RSat::KItems|RSat::KItemIconIds)
#define DSATTSY_ITEMS_ACTIONS_AND_ICONS	(RSat::KItems|RSat::KItemNextActions|RSat::KItemIconIds)
#define DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES				(RSat::KItems|RSat::KItemTextAttributes)
#define DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_AND_ACTIONS	(RSat::KItems|RSat::KItemNextActions|RSat::KItemTextAttributes)
#define DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_AND_ICONS			(RSat::KItems|RSat::KItemIconIds|RSat::KItemTextAttributes)
#define DSATTSY_ITEMS_WITH_TEXT_ATTRIBUTES_ACTIONS_AND_ICONS	(RSat::KItems|RSat::KItemNextActions|RSat::KItemIconIds|RSat::KItemTextAttributes)

#define DSATTSY_DISPLAY_TEXT_NUMBER				(0x01)
#define DSATTSY_GET_INKEY_NUMBER				(0x02)
#define DSATTSY_GET_INPUT_NUMBER				(0x03)
#define DSATTSY_PLAY_TONE_NUMBER				(0x04)
#define DSATTSY_SET_UP_MENU_NUMBER				(0x05)
#define DSATTSY_SELECT_ITEM_NUMBER				(0x06)
#define DSATTSY_SEND_SM_NUMBER					(0x07)
#define DSATTSY_SEND_SS_NUMBER					(0x08)
#define DSATTSY_SEND_USSD_NUMBER				(0x09)
#define DSATTSY_SET_UP_CALL_NUMBER				(0x0A)
#define DSATTSY_REFRESH_NUMBER					(0x0B)
#define DSATTSY_SET_UP_EVENT_LIST_NUMBER		(0x0C)
#define DSATTSY_SET_UP_IDLE_MODE_TEXT_NUMBER	(0x0D)
#define DSATTSY_SEND_DTMF_NUMBER				(0x0E)
#define DSATTSY_PERFORM_CARD_APDU_NUMBER		(0x0F)
#define DSATTSY_POWER_OFF_CARD_NUMBER			(0x10)
#define DSATTSY_POWER_ON_CARD_NUMBER			(0x20)
#define DSATTSY_GET_READER_STATUS_NUMBER		(0x30)
#define DSATTSY_RUN_AT_COMMAND_NUMBER			(0x40)
#define DSATTSY_LANGUAGE_NOTIFICATION_NUMBER	(0x50)
#define DSATTSY_LAUNCH_BROWSER_NUMBER			(0x60)
#define DSATTSY_CLOSE_CHANNEL_NUMBER			(0x70)
#define DSATTSY_RECEIVE_DATA_NUMBER				(0x80)
#define DSATTSY_SEND_DATA_NUMBER				(0x90)
#define DSATTSY_GET_CHANNEL_STATUS_NUMBER		(0xA0)
#define DSATTSY_SERVICE_SEARCH_NUMBER			(0xB0)
#define DSATTSY_GET_SERVICE_INFO_NUMBER			(0xC0)
#define DSATTSY_DECLARE_SERVICE_NUMBER			(0xD0)
#define DSATTSY_OPEN_CHANNEL_NUMBER				(0xE0)
#define DSATTSY_TIMER_MNGT_NUMBER				(0xE1)
#define DSATTSY_LOCAL_INFO_NUMBER				(0xE2)
#define DSATTSY_POLLING_INTERVAL_NUMBER			(0xE3)
#define DSATTSY_POLLING_OFF_NUMBER				(0xE4)
#define DSATTSY_RETRIEVE_MULTIMEDIA_MESSAGE		(0xE5)
#define DSATTSY_SUBMIT_MULTIMEDIA_MESSAGE		(0xE6)
#define DSATTSY_DISPLAY_MULTIMEDIA_MESSAGE		(0xE7)
#define DSATTSY_SET_FRAMES						(0xE8)
#define DSATTSY_GET_FRAMES_STATUS				(0xE9)        
        
        
#define DSATTSY_CARD_READER_1			RSat::KAdditionalCardReader1
#define DSATTSY_CLA						(1)
#define DSATTSY_INS						(2)
#define DSATTSY_LC						(3)
#define DSATTSY_LE						(4)
#define DSATTSY_P1						(5)
#define DSATTSY_P2						(6)
#define DSATTSY_CAPDU					_L8("CAPDU")

#define DSATTSY_GET_READER_STATUS_MODE	RSat::ECardReaderStatus

#define DSATTSY_AT_COMMAND				_L8("AT COMMAND")

#define DSATTSY_SPECIFIC_LANGUAGE		RSat::ESpecificLangNotification
#define DSATTSY_FRENCH					(0x1234)

#define DSATTSY_BROWSER_SELECTION_MODE	RSat::ELaunchBrowserIfNotAlreadyLaunched
#define DSATTSY_BROWSER_ID				RSat::EDefaultBrowser
#define DSATTSY_URL						_L8("www.symbian.com")
#define DSATTSY_BEARER_LIST				_L8("B1B2")
#define	DSATTSY_PROXY_NAME				_L("Proxy 1")
#define DSATTSY_NUM_OF_FILE_REF			(2)
/****
#define DSATTSY_FILE1					_L8("File1")
#define DSATTSY_FILE2					_L8("File2")
****/

#define DSATTSY_FILE1					_L8("File1")
#define DSATTSY_FILE2					_L8("File2")
#define	DSATTSY_FILE_NULL				_L8("\0")
// Provisioning file refs
// 1 = 1 entry:   [1 byte len, 1 byte ref]
// 2 = 1 entry:   [2 byte len, 128 byte ref]
// 3 = 2 entry:   [1 byte len, 1 byte ref],   [1 byte len, 1 byte ref]
// 4 = 1 entry:   [2 byte len, 128 byte ref]
// 4 = 2 entry:   [2 byte len, 128 byte ref], [1 byte len, 1 byte ref]

#define	DSATTSY_FILE_REF_DATA_1			(_L8("a"))			// 1-byte key
#define	DSATTSY_FILE_REF_DATA_128		(_L8("128bytes9.123456789.123456789.123456789.123456789.123456789.123456789.123456789.123456789.123456789.123456789.123456789.12345678"))

#define DSATTSY_INVALID_REF_FILE		(_L8("Invalid file"))

// Expected total lengths of decoded refs
#define	DSATTSY_REF_TOTAL_1				(1)
#define	DSATTSY_REF_TOTAL_2				(128)
#define	DSATTSY_REF_TOTAL_3				(2)
#define	DSATTSY_REF_TOTAL_4				(128)
#define	DSATTSY_REF_TOTAL_5				(129)

// Expected counts of decoded refs
#define DSATTSY_REF_COUNT_1				(1)
#define DSATTSY_REF_COUNT_2				(1)
#define DSATTSY_REF_COUNT_3				(2)
#define DSATTSY_REF_COUNT_4				(1)
#define DSATTSY_REF_COUNT_5				(2)

#define DSATTSY_DATA_LENGTH				(3)

#define DSATTSY_SEND_DATA_MODE			RSat::ESendDataImmediately
#define DSATTSY_CHANNEL_DATA			_L8("Channel Data")

#define DSATTSY_DECLARE_SERVICE_TYPE	RSat::EAddService
#define DSATTSY_NULL_NARROW_BUF			_L8("")
#define DSATTSY_BEARER_NOT_PRESENT		RSat::ELocalBearerIdNotPresent
#define DSATTSY_NULL_BEARER_ID			(0)
#define DSATTSY_TCP_PROTO				RSat::ETcp
#define DSATTSY_PORT_1					(1)

#define DSATTSY_BEARER_TECH_INDEPENDENT	RSat::ETechnologyIndependent
#define DSATTSY_SERVICE_SEARCH_PARAMS	_L8("Search Params")
#define DSATTSY_FILTER_INFO				_L8("Filter Info")

#define DSATTSY_ATTRIBUTE_INFO			_L8("Attribute Info")

#define DSATTSY_CS_BEARER				RSat::ECsBearer
#define DSATTSY_GPRS_BEARER				RSat::EGprsBearer
#define DSATTSY_LOCAL_BEARER			RSat::ELocalLinksBearer
#define	DSATTSY_ANY_BEARER				RSat::EAnyBearer
#define DSATTSY_ON_DEMAND_ESTABLISHMENT	RSat::EOnDemand
#define DSATTSY_AUTO_RECONNECTION		RSat::ENoAutomaticReconnection

#define DSATTSY_CSD_BEARER_TYPE			RSat::ECSDBearer
#define DSATTSY_BEARER_PARAMS			_L8("Bearer Params")
#define DSATTSY_BUFFER_SIZE				(2)
#define DSATTSY_IPV4_TYPE				RSat::EIPv4Address
#define DSATTSY_ADDRESS					_L8("987654321")
#define DSATTSY_LOGIN					_L("Login")
#define DSATTSY_PASSWORD				_L("Password")

#define DSATTSY_UICC_SERVER_MODE		RSat::EUiccServerMode

#define DSATTSY_ACCESS_NAME				_L8("Access Name")

#define DSATTSY_CODING					RSat::E48BitAddress

#define DSATTSY_NUMBER_OF_CHANNELS		(1)
#define DSATTSY_SCREEN_HEIGHT			(2)
#define DSATTSY_SCREEN_WIDTH			(3)
#define DSATTSY_PROTO_VERSION			(4)
#define DSATTSY_WIDTH_REDUCTION			(5)

#define DSATTSY_GET_INKEY_RSP_MODE		RSat::EImmediateDigitRsp

#define DSATTSY_SELECTION_PREFERENCE	RSat::ESoftKeyPreferred

#define DSATTSY_AID						_L8("AID")

#define	DSATTSY_EVENT_CARD_READER_STATUS		(0)
#define	DSATTSY_EVENT_LANGUAGE_SELECTION		(0)
#define	DSATTSY_EVENT_BROWSER_TERMINATION_CAUSE	(RSat::EUserTermination)
#define	DSATTSY_EVENT_DATA_AVAILABLE_STATUS		(_L8("\0\0"))
#define DSATTSY_EVENT_DATA_AVAILABLE_LENGTH		(0)
#define DSATTSY_EVENT_CHANNEL_STATUS			(_L8("\0\0"))
#define	DSATTSY_EVENT_DISPLAY_PROF14			(2)
#define DSATTSY_EVENT_DISPLAY_PROF15			(4)
#define DSATTSY_EVENT_DISPLAY_PROF16			(8)
#define DSATTSY_EVENT_LOCAL_BEARER_ID			(RSat::ELocalBearerIdNotSet)
#define DSATTSY_EVENT_LOCAL_SERVICE_ID			(0)
#define DSATTSY_EVENT_LOCAL_SERVICE_RECORD		(_L8("\0"))
#define DSATTSY_EVENT_LOCAL_ADDR_CODING			(RSat::E48BitAddress)
#define DSATTSY_EVENT_LOCAL_ADDRESS				(DSATTSY_ADDRESS)

#define DSATTSY_INFO_TYPE						(5)
#define DSATTSY_CB_PDU							_L8("CB PAGE")

#define DSATTSY_SENDSS			(RSat::ESendSs)

/************************
Testing RSat  V6 Features
*************************/

//Testing Notify Retrieve Multimedia Message Pcmd

#define DSATTSYV6_DEVICE_ID1			(RSat::KNetwork)
#define DSATTSYV6_DEVICE_ID2			(RSat::KUICC )
#define DSATTSYV6_DEVICE_ID3			(RSat::KME)
#define DSATTSYV6_DEVICE_ID4			(RSat::KDeviceIdNotSet)

#define DSATTSYV6_ALPHAID_STATUS1			(RSat::EAlphaIdNotSet)
#define DSATTSYV6_ALPHAID_STATUS2			(RSat::EAlphaIdProvided)
#define DSATTSYV6_ALPHAID1                  _L("ALPHA ID1")
#define DSATTSYV6_ALPHAID2                  _L("ALPHA ID2")
#define DSATTSYV6_ICON_QUALIFIER1           (RSat::EIconQualifierNotSet)
#define DSATTSYV6_ICON_QUALIFIER2           (RSat::ENoIconId)
#define DSATTSYV6_ICON_QUALIFIER3           (RSat::ESelfExplanatory)
#define DSATTSYV6_ICON_ID1                  (1)
#define DSATTSYV6_ICON_ID2                  (2)
#define DSATTSYV6_MULTIMEDIA_MESSAGE_REF1     _L("MESSAGE REFERENCE1")
#define DSATTSYV6_MULTIMEDIA_MESSAGE_REF2     _L("MESSAGE REFERENCE2")
#define DSATTSYV6_RECEPTION_FILE1           _L("RECEPTION FILE1 ")
#define DSATTSYV6_RECEPTION_FILE2           _L("RECEPTION FILE2 ")
#define DSATTSYV6_MULTIMEDIA_CONTENT_ID1         _L("MULTIMEDIA_CONTENT_ID1")
#define DSATTSYV6_MULTIMEDIA_CONTENT_ID2         _L("MULTIMEDIA_CONTENT_ID2")
#define DSATTSYV6_MULTIMEDIA_MESSAGE_ID1         _L("MULTIMEDIA_MESSAGE_ID1 ")
#define DSATTSYV6_MULTIMEDIA_MESSAGE_ID2         _L("MULTIMEDIA_MESSAGE_ID2 ")


//Testing Notify Submit Multimedia Message Pcmd
#define DSATTSYV6_SUBMISSION_FILE1           _L("SUBMISSION_FILE1 ")
#define DSATTSYV6_SUBMISSION_FILE2           _L("SUBMISSION_FILE2 ")

//Testing Notify Display Multimedia Message Pcmd
#define DSATTSYV6_DISPLAY_PRIORITY1             (RSat::ENormalPriority)
#define DSATTSYV6_DISPLAY_PRIORITY2             (RSat::EHighPriority )
#define DSATTSYV6_IMMEDIATE_RESPONSE1			(RSat::EImmediateRspNotSet)
#define DSATTSYV6_IMMEDIATE_RESPONSE2			(RSat::ENoImmediateRsp )

//Testing Set Frames Pcmd
#define DSATTSYV6_FRAME_LAYOUT_FORMAT1			(RSat::EFrameHorizontal)
#define DSATTSYV6_FRAME_LAYOUT_FORMAT2			(RSat::EFrameVertical)
#define DSATTSYV6_FRAME_LAYOUT1		            _L("FRAME_LAYOUT1 ")
#define DSATTSYV6_FRAME_LAYOUT2                 _L("FRAME_LAYOUT2 ")
#define DSATTSYV6_FRAME_IDENTIFIER1             (1)
#define DSATTSYV6_FRMAE_IDENTIFIER2             (2)
#define DSATTSYV6_DEFAULT_FRAME_IDENTIFIER1             (1)
#define DSATTSYV6_DEFAULT_FRMAE_IDENTIFIER2             (2)

//Testing MmsNotificationDownload
#define DSATTSYV6_LAST_ENVELOPE1             (1)
#define DSATTSYV6_LAST_ENVELOPE2             (2)
#define DSATTSYV6_MMS_NOTIFICATION1		            _L("MMS_NOTIFICATION1 ")
#define DSATTSYV6_MMS_NOTIFICATION2                 _L("MMS_NOTIFICATION2 ")

//Testing MmsTransferStatus
#define DSATTSYV6_TRANSFER_FILE1                     _L("TRANSFER_FILE1 ")
#define DSATTSYV6_MULTIMEDIA_MESSAGE_STATUS          _L("MULTIMEDIA_MESSAGE_STATUS")

//Local Info V6
#define DSATTSYV6_LOCAL_INFOV6               (RSat::EUtranMeasurementQualifierIntraFrequency)

//Play Tone V6
#define DSATTSY_TONEV6					     (RSat::KMelody7)

//Network Search Mode Event
#define DSATTSY_NETWORK_SEARCH_MODE1         (RSat::EManual)
#define DSATTSY_NETWORK_SEARCH_MODE2         (RSat::EAutomatic)

//Browsing Status Change Event
#define DSATTSY_BROWSING_STATUS1      (1)
#define DSATTSY_BROWSING_STATUS2      (2)

//Frames Information Change Event
#define DSATTSY_FRAMES_INFORMATION_FRAMEID      (0x01)
#define DSATTSY_FRAMES_INFORMATION_FRAME_LIST     _L8("FRAME LIST")

//Open Channel

#define DSATTSYV6_GPRS_BEARER_TYPE1			RSat::EUtranBearer

#endif // __TESTDEF_H


