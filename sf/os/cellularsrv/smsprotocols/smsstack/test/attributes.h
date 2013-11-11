/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* defines attributes used by Dummy TSY
* 
*
*/



/**
 @file
 @internalComponent
*/

#ifndef __ATTRIBUTES_H
#define __ATTRIBUTES_H

#include "etelmm.h"


// Testing SMS Messaging

// SMS CAPS
#define DMMTSY_SMS_MESSAGING_MODE_CAPS_0					(TUint32)(RMobileSmsMessaging::KCapsGsmSms |\
																RMobileSmsMessaging::KCapsCdmaSms)

#define DMMTSY_SMS_MESSAGING_MODE_CAPS_1					(TUint32)(RMobileSmsMessaging::KCapsGsmSms)
#define DMMTSY_SMS_MESSAGING_MODE_CAPS_2					(TUint32)(RMobileSmsMessaging::KCapsCdmaSms)


#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_0					(TUint32)(RMobileSmsMessaging::KCapsReceiveStored |\
															RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck |\
															RMobileSmsMessaging::KCapsReceiveUnstoredClientAck |\
															RMobileSmsMessaging::KCapsSendWithAck |\
															RMobileSmsMessaging::KCapsGetSmspList |\
															RMobileSmsMessaging::KCapsSetSmspList)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_1					(TUint32)(RMobileSmsMessaging::KCapsReceiveStored |\
															RMobileSmsMessaging::KCapsSendWithAck)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_2					(TUint32)(RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck |\
															RMobileSmsMessaging::KCapsSendWithAck |\
															RMobileSmsMessaging::KCapsGetSmspList|\
															RMobileSmsMessaging::KCapsSetSmspList)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_3					(TUint32)(RMobileSmsMessaging::KCapsSendNoAck |\
															RMobileSmsMessaging::KCapsGetSmspList |\
															RMobileSmsMessaging::KCapsSetSmspList)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_4					(TUint32)(RMobileSmsMessaging::KCapsReceiveEither |\
															RMobileSmsMessaging::KCapsSendWithAck |\
															RMobileSmsMessaging::KCapsGetSmspList |\
															RMobileSmsMessaging::KCapsSetSmspList)

#define DMMTSY_SMS_MESSAGING_CONTROL_CAPS_5					(TUint32)(RMobileSmsMessaging::KCapsReceiveUnstoredClientAck |\
															RMobileSmsMessaging::KCapsSendWithAck |\
															RMobileSmsMessaging::KCapsGetSmspList |\
															RMobileSmsMessaging::KCapsSetSmspList)

// SMS RECEIVE MODE
#define DMMTSY_SMS_MESSAGING_RECEIVE_MODE_1					RMobileSmsMessaging::EReceiveUnstoredClientAck
#define DMMTSY_SMS_MESSAGING_RECEIVE_MODE_2					RMobileSmsMessaging::EReceiveStored

// SMS BEARER
#define DMMTSY_SMS_MESSAGING_BEARER_1						RMobileSmsMessaging::ESmsBearerPacketPreferred
#define DMMTSY_SMS_MESSAGING_BEARER_2						RMobileSmsMessaging::ESmsBearerCircuitOnly

// RECEIVE ATTRIBUTES
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_0						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KGsmServiceCentre |\
															 RMobileSmsMessaging::KStorageLocation )
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_1						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KGsmServiceCentre |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KIncomingStatus |\
															 RMobileSmsMessaging::KStorageLocation )
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_2						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KIncomingStatus |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_3						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_4						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_5						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_6						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_7						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)
#define DMMTSY_SMS_ATTRIBUTE_FLAGS_8						(TUint32)(RMobileSmsMessaging::KSmsDataFormat |\
															 RMobileSmsMessaging::KRemotePartyInfo |\
															 RMobileSmsMessaging::KStorageLocation |\
															 RMobileSmsMessaging::KGsmServiceCentre)

// Includes Type of number, Numbering plan and the actual number
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_0			(RMobilePhone::EUnknownNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_0			(RMobilePhone::EUnknownNumberingPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_0	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_1			(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_1			(RMobilePhone::EIsdnNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_1	_L("358405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_2			(RMobilePhone::ENationalNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_2			(RMobilePhone::EDataNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_2	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_3			(RMobilePhone::ENetworkSpecificNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_3			(RMobilePhone::ETelexNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_3	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_4			(RMobilePhone::ESubscriberNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_4			(RMobilePhone::EServiceCentreSpecificPlan1)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_4	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_5			(RMobilePhone::EAlphanumericNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_5			(RMobilePhone::EServiceCentreSpecificPlan2)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_5	_L("SONERA")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_6			(RMobilePhone::EAbbreviatedNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_6			(RMobilePhone::ENationalNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_6	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_7			(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_7			(RMobilePhone::EPrivateNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_7	_L("")

#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TON_8			(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_NP_8			(RMobilePhone::EERMESNumberPlan)
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_8	_L("358405202222")
#define DMMTSY_SMS_MESSAGE_GSMSERVICECENTRE_TEL_NUMBER_9	_L("358405202555")

// Data Format
#define DMMTSY_SMS_MESSAGE_DATAFORMAT_0						(RMobileSmsMessaging::EFormatUnspecified)
#define DMMTSY_SMS_MESSAGE_DATAFORMAT_1						(RMobileSmsMessaging::EFormatGsmTpdu)
#define DMMTSY_SMS_MESSAGE_DATAFORMAT_2						(RMobileSmsMessaging::EFormatCdmaTpdu)

// Originator includes Type of number, Numbering plan and the actual number
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_0					(RMobilePhone::EUnknownNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_0					(RMobilePhone::EUnknownNumberingPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_0			_L("0405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_1					(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_1					(RMobilePhone::EIsdnNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_1			_L("358405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_2					(RMobilePhone::ENationalNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_2					(RMobilePhone::EDataNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_2			_L("0405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_3					(RMobilePhone::ENetworkSpecificNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_3					(RMobilePhone::ETelexNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_3			_L("0405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_4					(RMobilePhone::ESubscriberNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_4					(RMobilePhone::EServiceCentreSpecificPlan1)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_4			_L("0405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_5					(RMobilePhone::EAlphanumericNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_5					(RMobilePhone::EServiceCentreSpecificPlan2)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_5			_L("0405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_6					(RMobilePhone::EAbbreviatedNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_6					(RMobilePhone::ENationalNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_6			_L("358405202000")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_7					(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_7					(RMobilePhone::EPrivateNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_7			_L("")

#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TON_8					(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_NP_8					(RMobilePhone::EERMESNumberPlan)
#define DMMTSY_SMS_MESSAGE_ORIGINATOR_TEL_NUMBER_8			_L("358405202000")

// Mobile Sms Incoming Status
#define DMMTSY_SMS_MESSAGE_STATUS_0							(RMobileSmsMessaging::EMtMessageUnknownStatus)
#define DMMTSY_SMS_MESSAGE_STATUS_1							(RMobileSmsMessaging::EMtMessageUnstoredPhoneAck)
#define DMMTSY_SMS_MESSAGE_STATUS_2							(RMobileSmsMessaging::EMtMessageUnstoredClientAck)
#define DMMTSY_SMS_MESSAGE_STATUS_3							(RMobileSmsMessaging::EMtMessageStored)

// Store index
#define DMMTSY_SMS_MESSAGE_STOREINDEX_0						(1)
#define DMMTSY_SMS_MESSAGE_STOREINDEX_1						(2)
#define DMMTSY_SMS_MESSAGE_STOREINDEX_2						(3)
#define DMMTSY_SMS_MESSAGE_STOREINDEX_3						(4)


//Deliver report
#define DMMTSY_SMS_DEFAULT_DELIVER_REPORT					(TText8*)("0000") //Default deliver report

// SEND ATTRIBUTES
//Flags
#define DMMTSY_SMS_SEND_ATTRIBUTE_FLAGS_0					(TUint32)(RMobileSmsMessaging::KMessageReference |\
															 RMobileSmsMessaging::KGsmSubmitReport )
#define DMMTSY_SMS_SEND_ATTRIBUTE_FLAGS_1					(TUint32)(RMobileSmsMessaging::KMessageReference)
#define DMMTSY_SMS_SEND_ATTRIBUTE_FLAGS_2					(TUint32)(RMobileSmsMessaging::KGsmSubmitReport)


// Message references are handled by using internal member attribute
#define DMMTSY_SMS_MESSAGE_MSGREF_0							(0)
#define DMMTSY_SMS_MESSAGE_MSGREF_1							(1)
#define DMMTSY_SMS_MESSAGE_MSGREF_2							(2)

#define DMMTSY_SMS_MESSAGE_MSGREF_3							(34)
#define DMMTSY_SMS_MESSAGE_MSGREF_4							(255)

// Submit reports
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_0					(TText8*)("010010807141731480")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_1					(TText8*)("010010801241332180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_2					(TText8*)("010010101000000080")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_3					(TText8*)("010010111111111180")

#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_4					(TText8*)("010010112051731180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_5					(TText8*)("010010112051733180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_6					(TText8*)("010010112051735180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_7					(TText8*)("010010112051731480")

//Submit reports with RP-Cause
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_8					(TText8*)("01080010112211221180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_21					(TText8*)("01150010801241332180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_27					(TText8*)("011B0010112211221180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_28					(TText8*)("011C0010801241332180")
#define DMMTSY_SMS_MESSAGE_SUBMITREPORT_29					(TText8*)("011D0010112211221180")
//																		 |
//																		 RP-CAUSE

#define DMMTSY_SMS_MESSAGE_MORE_0							(TBool)(EFalse)
#define DMMTSY_SMS_MESSAGE_MORE_1							(TBool)(ETrue)

//SMS STORAGE

#define DMMTSY_PHONE_MESSAGE_STORE_COUNT_0					(2)
#define DMMTSY_PHONE_MESSAGE_STORE_COUNT_1					(4)
// Store index
#define DMMTSY_SMS_STORE_INDEX_1							(30)
#define DMMTSY_SMS_STORE_INDEX_2							(45)
#define DMMTSY_SMS_STORE_INDEX_3							(101)
#define DMMTSY_SMS_STORE_INDEX_4							(127)

#define DMMTSY_SMS_MESSAGE_FULL								(TBool)(ETrue)

#define DMMTSY_SMS_MESSAGE_FORMAT1							RMobileSmsMessaging::EFormatGsmTpdu
#define DMMTSY_SMS_MESSAGE_STATUS1							RMobileSmsMessaging::EMtMessageStored

#define DMMTSY_SMS_MESSAGE_FORMAT2							RMobileSmsMessaging::EFormatCdmaTpdu
#define DMMTSY_SMS_MESSAGE_TELESERVICE						(TInt)(34)
#define DMMTSY_SMS_MESSAGE_STATUS2							RMobileSmsMessaging::EMtMessageUnstoredClientAck

//SMS TPDUS FOR SMS STORAGE TESTING
// 8 bit rx pdu, contains:"test message, 8bits, length 30"
#define DMMTSY_SMS_MESSAGE_PDU_1							(TText8*)("240C915348801455820004108071417314801E74657374206D6573736167652C2038626974732C206C656E677468203330") //8bit pdu
// 7bit rx pdu contains: "test message, length 23"
#define DMMTSY_SMS_MESSAGE_PDU_2							(TText8*)("240C9153488014558200001080714173148017F4F29C0E6A97E7F3F0B9CC02B1CBEE331D0D92CD00") //7bit pdu
//Class 0 rx pdu, contains: "Class 0 message"
#define DMMTSY_SMS_MESSAGE_PDU_3							(TText8*)("040C915348801455820010108032110222800F4376783E07C140EDF27C1E3E9701")

// Three pdus of one concatenated message that contains:
//"3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. 3 PDU test SMS message. The End."
#define DMMTSY_SMS_MESSAGE_CONCATENATED1					(TText8*)"440C915348801455820000103060019153009F050003000301662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA29701"
#define DMMTSY_SMS_MESSAGE_CONCATENATED2					(TText8*)"440C915348801455820000103060019193009F050003000302E674D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B501"
#define DMMTSY_SMS_MESSAGE_CONCATENATED3					(TText8*)"440C9153488014558200001030600191340077050003000303CAF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681662028B10AA297E774D0B43905B5CBF379F85C7681A8E832A8E826BB00"

//SMS STORAGE
//7-bit message, contains:"ME STORE MESSAGE 1"
#define DMMTSY_SMS_ME_STORE_PDU_1							(TText8*)("15000C915348801455820000A712CD22684A7D4A8BA066713A0D1E8BA018")
//7-bit message, contains:"ME STORE MESSAGE 2"
#define DMMTSY_SMS_ME_STORE_PDU_2							(TText8*)("15000C915348801455820000A712CD22684A7D4A8BA066713A0D1E8B2019")

//7-bit message, contains:"SIM STORE MESSAGE 1"
#define DMMTSY_SMS_SIM_STORE_PDU_1							(TText8*)("240C9153488014558200001011919111228013D3641334A53EA54550B3389D068F45500C")
//7-bit message, contains:"SIM STORE MESSAGE 2"
#define DMMTSY_SMS_SIM_STORE_PDU_2							(TText8*)("240C9153488014558200001011919191228013D3641334A53EA54550B3389D068F45900C")
//7-bit message, contains:"SIM STORE MESSAGE 3"
#define DMMTSY_SMS_SIM_STORE_PDU_3							(TText8*)("240C9153488014558200001011919191228013D3641334A53EA54550B3389D068F45C00C")

#define DMMTSY_MESSAGE_STORE_CAPS_1							(TUint32)(RMobilePhoneStore::KCapsNotifyEvent |\
															RMobileSmsStore::KCapsGsmMessages)

#define DMMTSY_MESSAGE_STORE_CAPS_2							(TUint32)(RMobilePhoneStore::KCapsWholeStore |\
															RMobilePhoneStore::KCapsReadAccess |\
															RMobilePhoneStore::KCapsWriteAccess |\
															RMobilePhoneStore::KCapsNotifyEvent |\
															RMobileSmsStore::KCapsReadMessages |\
															RMobileSmsStore::KCapsUnreadMessages |\
															RMobileSmsStore::KCapsSentMessages |\
															RMobileSmsStore::KCapsUnsentMessages |\
															RMobileSmsStore::KCapsGsmMessages)

#define DMMTSY_MESSAGE_STORE_CAPS_3							(TUint32)(RMobilePhoneStore::KCapsIndividualEntry |\
															RMobilePhoneStore::KCapsReadAccess |\
															RMobilePhoneStore::KCapsWriteAccess |\
															RMobilePhoneStore::KCapsDeleteAll |\
															RMobilePhoneStore::KCapsNotifyEvent)

#define DMMTSY_MESSAGE_STORE_CAPS_4							(TUint32)(RMobilePhoneStore::KCapsIndividualEntry |\
															RMobilePhoneStore::KCapsReadAccess |\
															RMobilePhoneStore::KCapsWriteAccess |\
															RMobilePhoneStore::KCapsNotifyEvent)

#define DMMTSY_MESSAGE_STORE_EVENT							RMobilePhoneStore::KStoreHasSpace

//OTHER DEFINITIONS
#define DMMTSY_SMS_MESSAGE_NACK_CAUSE						(-4534) // KErrGsmSMSMemoryCapacityExceeded

// SMS PARAMETERS STORAGE
#define DMMTSY_SMSP_STORE_SCA								_L("02075474324")
#define DMMTSY_SMSP_STORE_EVENT							    RMobilePhoneStore::EStoreFull
#define DMMTSY_SMSP_STORE_INDEX1							(1)
#define DMMTSY_SMSP_STORE_TEXT1								_L("Katie's SMS address")
#define DMMTSY_SMSP_STORE_VALID_PARAMS1						(TUint8)(0x1F)
#define DMMTSY_SMSP_STORE_PID1								(TUint8)(0x34)
#define DMMTSY_SMSP_STORE_DCS1								(TUint8)(0x8A)
#define DMMTSY_SMSP_STORE_VALIDITY1							(TUint8)(0x91)

#define DMMTSY_SMSP_STORE_INDEX2							(2)
#define DMMTSY_SMSP_STORE_TEXT2								_L("Ian's SMS address")
#define DMMTSY_SMSP_STORE_VALID_PARAMS2						(TUint8)(0x1F)
#define DMMTSY_SMSP_STORE_PID2								(TUint8)(0x73)
#define DMMTSY_SMSP_STORE_DCS2								(TUint8)(0xF2)
#define DMMTSY_SMSP_STORE_VALIDITY2							(TUint8)(0x04)

#define DMMTSY_SMSP_STORE_STATUS							(TBool)(EFalse)
#define DMMTSY_SMSP_STORE_COUNT								(2)

// Destination includes Type of number, Numbering plan and the actual number
#define DMMTSY_SMS_MESSAGE_DESTINATION_TON_0				(RMobilePhone::EInternationalNumber)
#define DMMTSY_SMS_MESSAGE_DESTINATION_NP_0					(RMobilePhone::EIsdnNumberPlan)
#define DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_0			_L("358405202000")

#define DMMTSY_SMS_MESSAGE_DESTINATION_TON_1			(RMobilePhone::ENationalNumber)
#define DMMTSY_SMS_MESSAGE_DESTINATION_NP_1			(RMobilePhone::EDataNumberPlan)
#define DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_1	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_DESTINATION_TON_2			(RMobilePhone::ENetworkSpecificNumber)
#define DMMTSY_SMS_MESSAGE_DESTINATION_NP_2			(RMobilePhone::ETelexNumberPlan)
#define DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_2	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_DESTINATION_TON_3			(RMobilePhone::ESubscriberNumber)
#define DMMTSY_SMS_MESSAGE_DESTINATION_NP_3			(RMobilePhone::EServiceCentreSpecificPlan1)
#define DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_3	_L("0405202000")

#define DMMTSY_SMS_MESSAGE_DESTINATION_TON_4			(RMobilePhone::EAlphanumericNumber)
#define DMMTSY_SMS_MESSAGE_DESTINATION_NP_4			(RMobilePhone::EServiceCentreSpecificPlan2)
#define DMMTSY_SMS_MESSAGE_DESTINATION_TEL_NUMBER_4	_L("0405202000")

#endif
