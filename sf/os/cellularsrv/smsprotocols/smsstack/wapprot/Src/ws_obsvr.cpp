// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements CWapSapMessageSender and CWapProtocolObserver
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ws_obsvrTraces.h"
#endif

#include "ws_obsvr.h"

#include <commsdattypesv1_1.h>

#include "smsprot.h"
#include "ws_main.h"
#include "wapmain.h"
#include "WAPDGRM.H"
#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

using namespace CommsDat;

//
// Constant values
//
// KWapSmsmessageValidityPeriod - Hardcoded message validity period (in minutes)
//                                for WAPSMS and WAPSMS7 bearers.
//
//

const TInt KWapSmsMessageValidityPeriod = 5;
const TInt KSmsMsgArraySegmentSize = 8;

//
// CWapSapMessageSender
//
// Owned by CWapSmsProvider, implements MSmsMessageObserver.
// Splits up a CWapDatagram into one or more SMS messages and sends them to
// Sms.Prt one at a time.
//

_LIT(KWAPSMSBearer, "WAPSMSBearer");

/**
 *  Constructor
 */
CWapSapMessageSender::CWapSapMessageSender(CSmsProtocol* aSmsProtocol, CWapSmsProvider* aWapSap)
    {
    iSmsAddr.SetSmsAddrFamily(ESmsAddrSendOnly);
    iSmsProtocol = aSmsProtocol;
    iProvider = aWapSap;
    } // CWapSapMessageSender::CWapSapMessageSender


/**
 *  Destructor
 */
CWapSapMessageSender::~CWapSapMessageSender()
    {
    if (iSmsProtocol)
        {
        iSmsProtocol->RemoveSmsMessageObserver(*this);
        }

    delete iSmsMsgArray;
    } // CWapSapMessageSender::~CWapSapMessageSender


/**
 *  Factory
 */
CWapSapMessageSender* CWapSapMessageSender::NewL(CSmsProtocol* aSmsProtocol, CWapSmsProvider* aWapSap)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_NEWL_1, "CWapSapMessageSender::NewL");

	CWapSapMessageSender* msgSender = new(ELeave) CWapSapMessageSender(aSmsProtocol,aWapSap);
	CleanupStack::PushL(msgSender);
	msgSender->iSmsMsgArray = new(ELeave) CArrayPtrFlat<CSmsMessage>(KSmsMsgArraySegmentSize);
	aSmsProtocol->AddSmsMessageObserverL(*msgSender);
	User::LeaveIfError(aSmsProtocol->BindSmsMessageObserver(*msgSender,msgSender->iSmsAddr));
	CleanupStack::Pop();
	return msgSender;
	} // CWapSapMessageSender::NewL


/**
 *  Send a message to the SMSPROT.PRT
 *  Splits up the CWapDatagram into one or more SMS messages and sends them
 *  one at a time to SMSPROT.PRT
 */
void CWapSapMessageSender::SendDatagramL(CWapDatagram* aMsg)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_SENDDATAGRAML_1, "CWapSapMessageSender::SendDatagramL");

	__ASSERT_DEBUG(iSmsMsgArray->Count()==0,Panic(EWapSmsSapMessageSenderBusy));

	CleanupStack::PushL(aMsg);
	aMsg->EncodeConcatenatedMessagesL(iSmsProtocol->FileSession(), *iSmsMsgArray);
	SendNextSms();
	CleanupStack::PopAndDestroy();
	} // CWapSapMessageSender::SendDatagramL


/**
 *  Send an SMS message to SMSPROT.PRT
 */
void CWapSapMessageSender::SendNextSms()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_SENDNEXTSMS_1, "CWapSapMessageSender::SendNextSms");

	__ASSERT_DEBUG(iSmsMsgArray->Count()>0,Panic(EWapSmsSapMessageSenderNothingToSend));
	CSmsMessage* smsMsg = iSmsMsgArray->At(0);
	TRAPD(ret, SetSmsMessageSettingsL(smsMsg) );
	if(ret != KErrNone)
		iProvider->Error(ret, MSocketNotify::EErrorSend);
	else
		{
		iSmsMsgArray->Delete(0);
		iSmsProtocol->SendSmsMessage(smsMsg,*this,0);
		}

	} // CWapSapMessageSender::SendNextSms


/**
 *  Setup the SMS message according to the settings stored in the CommDB
 */
void CWapSapMessageSender::SetSmsMessageSettingsL(CSmsMessage* aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_SETSMSMESSAGESETTINGSL_1, "CWapSapMessageSender::SetSmsMessageSettingsL");

	TBuf<KCommsDbSvrMaxFieldLength> msgCenterNumberValue;
	TUint32 msgValidityPeriodValue = KWapSmsMessageValidityPeriod;
	TBool msgDeliveryReportValue = EFalse;

	TUint32 modemId = 0;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(db);

	CSmsSubmit* smsSubmit = reinterpret_cast<CSmsSubmit*>(&aSmsMessage->SmsPDU());
	if (iProvider->MessageType()==CWapSmsProvider::ESmartMessage)
		{
		CMDBField<TUint32>* globalSettingsField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
		CleanupStack::PushL(globalSettingsField);
		globalSettingsField->SetRecordId(1);
		globalSettingsField->LoadL(*db);
		modemId = *globalSettingsField;
		CleanupStack::PopAndDestroy(globalSettingsField);

		CMDBField<TDesC>* msgCenterNumberValueField = new(ELeave) CMDBField<TDesC>(KCDTIdMessageCentreNumber);
		CleanupStack::PushL(msgCenterNumberValueField);
		msgCenterNumberValueField->SetRecordId(modemId);
		msgCenterNumberValueField->LoadL(*db);
		msgCenterNumberValue = *msgCenterNumberValueField;
		CleanupStack::PopAndDestroy(msgCenterNumberValueField);

		CMDBField<TUint32>* msgValidityPeriodValueField = new(ELeave) CMDBField<TUint32>(KCDTIdMessageValidityPeriod);
		CleanupStack::PushL(msgValidityPeriodValueField);
		msgValidityPeriodValueField->SetRecordId(modemId);
		msgValidityPeriodValueField->LoadL(*db);
		msgValidityPeriodValue = *msgValidityPeriodValueField;
		CleanupStack::PopAndDestroy(msgValidityPeriodValueField);

		CMDBField<TBool>* msgDeliveryReportValueField = new(ELeave) CMDBField<TBool>(KCDTIdMessageDeliveryReport);
		CleanupStack::PushL(msgDeliveryReportValueField);
		msgDeliveryReportValueField->SetRecordId(modemId);
		msgDeliveryReportValueField->LoadL(*db);
		msgDeliveryReportValue = *msgDeliveryReportValueField;
		CleanupStack::PopAndDestroy(msgDeliveryReportValueField);

		TTimeIntervalMinutes validityPeriod(msgValidityPeriodValue);
		smsSubmit->SetValidityPeriod(validityPeriod);
		
		if (msgDeliveryReportValue)
			{
			if ((iProvider->GetStatusReportScheme() == EWapSmsTPSRR) && (iProvider->GetDataCodingScheme() == EWapSms8BitDCS))
				{
				CSmsTPSRROperations& operation = (CSmsTPSRROperations&)aSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter);
				operation.SetSchemeL();
				operation.SetLastSegmentStatusReportL(ETrue);
				}
			else 
				{
				smsSubmit->SetStatusReportRequest(msgDeliveryReportValue);	
				}	
			}
	
		}
	else // 8-bit WAP
		{
		CMDBField<TUint32>* globalSettingsField = new(ELeave) CMDBField<TUint32>(KCDTIdGlobalWapAcessPoint);
		CleanupStack::PushL(globalSettingsField);
		globalSettingsField->SetRecordId(1);
		globalSettingsField->LoadL(*db);
		modemId = *globalSettingsField;
		CleanupStack::PopAndDestroy(globalSettingsField);

		CMDBField<TDesC>* bearerField = new(ELeave) CMDBField<TDesC>(KCDTIdWAPCurrentBearer);
		CleanupStack::PushL(bearerField);
		bearerField->SetRecordId(modemId);
		bearerField->LoadL(*db);
		TBuf<KCommsDbSvrMaxFieldLength> bearerType;
		bearerType = *bearerField;
		CleanupStack::PopAndDestroy(bearerField);

		if (TPtrC(KWAPSMSBearer) != TPtrC(bearerType))
			{
			// We're not the current bearer - cancel msgs in send queue
			// and leave with not supported error indication
			iSmsMsgArray->ResetAndDestroy();
			User::Leave(KErrNotSupported);
			}

		CMDBField<TDesC>* centerNumberField = new(ELeave) CMDBField<TDesC>(KCDTIdWAPSMSServiceCentreAddress);
		CleanupStack::PushL(centerNumberField);
		centerNumberField->SetRecordId(modemId);
		centerNumberField->LoadL(*db);
		msgCenterNumberValue = *centerNumberField;
		CleanupStack::PopAndDestroy(centerNumberField);

		smsSubmit->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFNone);

		CMDBField<TUint32>* globalSettingsField2 = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
		CleanupStack::PushL(globalSettingsField2);
		globalSettingsField2->SetRecordId(1);
		globalSettingsField2->LoadL(*db);
		modemId = *globalSettingsField2;
		CleanupStack::PopAndDestroy(globalSettingsField2);

		CMDBField<TBool>* msgDeliveryReportValueField = new(ELeave) CMDBField<TBool>(KCDTIdMessageDeliveryReport);
		CleanupStack::PushL(msgDeliveryReportValueField);
		msgDeliveryReportValueField->SetRecordId(modemId);
		msgDeliveryReportValueField->LoadL(*db);
		msgDeliveryReportValue = *msgDeliveryReportValueField;
		CleanupStack::PopAndDestroy(msgDeliveryReportValueField);

		if (msgDeliveryReportValue)
			{
			if ((iProvider->GetStatusReportScheme() == EWapSmsTPSRR) && (iProvider->GetDataCodingScheme() == EWapSms8BitDCS))
				{
				CSmsTPSRROperations& operation = (CSmsTPSRROperations&)aSmsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter);
				operation.SetSchemeL();
				operation.SetLastSegmentStatusReportL(ETrue);
				}
			else
				{
				smsSubmit->SetStatusReportRequest(msgDeliveryReportValue);	
				}	
			}

		}
	CleanupStack::PopAndDestroy(); // db

	aSmsMessage->SmsPDU().SetServiceCenterAddressL(msgCenterNumberValue);
	} // CWapSapMessageSender::SetSmsMessageSettingsL


/**
 *  Received notification that a message has been sent.
 *  Continue sending SMSs if we have more or complete the send.
 */
void CWapSapMessageSender::MessageSendCompleted(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_MESSAGESENDCOMPLETED_1, "CWapSapMessageSender::MessageSendCompleted [aStatus=%d]", aStatus);

	if (aStatus!=KErrNone)
		iSmsMsgArray->ResetAndDestroy();

	if (iSmsMsgArray->Count()==0)
		{
		iProvider->Error(aStatus,MSocketNotify::EErrorSend);
		return;
		}

	SendNextSms();
	} // CWapSapMessageSender::MessageSendCompleted

/**
 *  Return the address
 */
const TSmsAddr& CWapSapMessageSender::GetLocalAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_GETLOCALADDRESS_1, "CWapSapMessageSender::GetLocalAddress()");

	return iSmsAddr;
	} // CWapSapMessageSender::GetLocalAddress


/**
 *  Set the address
 */
void CWapSapMessageSender::SetLocalAddress(const TSmsAddr& aSmsAddr)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_SETLOCALADDRESS_1, "CWapSapMessageSender::SetLocalAddress()");

	iSmsAddr = aSmsAddr;
	} // CWapSapMessageSender::SetLocalAddress


/**
 *  Notification that the modem has been connected or disconnected
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::ModemNotificationCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_MODEMNOTIFICATIONCOMPLETED_1, "CWapSapMessageSender::ModemNotificationCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that the enumeration has completed
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::EnumeratePhoneCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_ENUMERATEPHONECOMPLETED_1, "CWapSapMessageSender::EnumeratePhoneCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a message has been received
 *  Messages should never be directly received by a SAP
 */
TInt CWapSapMessageSender::MessageReceived(const CSmsMessage& /*aSmsMessage*/,TDes& /*aDes*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_MESSAGERECEIVED_1, "CWapSapMessageSender::MessageReceived");
    __ASSERT_DEBUG(EFalse,Panic(EWapSmsReceiveOnMessageSender));
    return KErrNone;
    BULLSEYE_RESTORE
    }

/**
 *  Informs protocol whether client confirms received message
 */
TBool CWapSapMessageSender::ClientConfirmsMessage()const
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_CLIENTCONFIRMSMESSAGE_1, "CWapSapMessageSender::ClientConfirmsMessage");
    __ASSERT_DEBUG(EFalse,Panic(EWapSmsReceiveOnMessageSender));
    return ETrue;
    BULLSEYE_RESTORE
    }

/**
 *  Informs protocol whether address is used by the observer
 */
TInt CWapSapMessageSender::SmsAddrIsDuplicate(const MSmsMessageObserver* /*aObserver*/,const TSmsAddr& /*aAddr*/)const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_SMSADDRISDUPLICATE_1, "CWapSapMessageSender::SmsAddrIsDuplicate()");

	return EFalse;
	} // CWapSapMessageSender::SmsAddrIsDuplicate


/**
 *  Notification that a message has been written to the phone or SIM
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::MessageWriteCompleted(TInt /*aStatus*/, const CSmsMessage* /*aSmsMessage*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_MESSAGEWRITECOMPLETED_1, "CWapSapMessageSender::MessageWriteCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a PDU delete has completed
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::MessageDeleteCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_MESSAGEDELETECOMPLETED_1, "CWapSapMessageSender::MessageDeleteCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a read SMS parameters has completed
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::ReadSmsParamsCompleted(TInt /*aStatus*/,CMobilePhoneSmspList* /*aSmspList*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_READSMSPARAMSCOMPLETED_1, "CWapSapMessageSender::ReadSmsParamsCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a write SMS parameters has completed
 *  (Has no relevance for the wap stack)
 */
void CWapSapMessageSender::WriteSmsParamsCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPSAPMESSAGESENDER_WRITESMSPARAMSCOMPLETED_1, "CWapSapMessageSender::WriteSmsParamsCompleted()");
    BULLSEYE_RESTORE
    }

//
// CWapProtocolObserver
//
// This class implements the MSmsMessageObserver interface defined by Sms.Prt
// Receives Sms messages from sms.prt.
//


/**
 *  Factory
 */
CWapProtocolObserver* CWapProtocolObserver::NewL(CWapSmsProtocol* aProtocol)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_NEWL_1, "CWapProtocolObserver::NewL()");

	CWapProtocolObserver* prot = new(ELeave) CWapProtocolObserver;
	prot->iWapSmsProtocol=aProtocol;
	return prot;
	} // CWapProtocolObserver::NewL


/**
 *  Return the address
 */
const TSmsAddr& CWapProtocolObserver::GetLocalAddress() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_GETLOCALADDRESS_1, "CWapProtocolObserver::GetLocalAddress()");

	return iSmsAddr;
	} // CWapProtocolObserver::GetLocalAddress


/**
 *  Set the address
 */
void CWapProtocolObserver::SetLocalAddress(const TSmsAddr& aSmsAddr)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_SETLOCALADDRESS_1, "CWapProtocolObserver::SetLocalAddress()");

	iSmsAddr = aSmsAddr;
	} // CWapProtocolObserver::SetLocalAddress


/**
 *  Notification that the modem has been connected or disconnected
 *  (Has no relevance for the wap stack)
 */
void CWapProtocolObserver::ModemNotificationCompleted(TInt /*aStatus*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_MODEMNOTIFICATIONCOMPLETED_1, "CWapProtocolObserver::ModemNotificationCompleted()");

	} // CWapProtocolObserver::ModemNotificationCompleted


/*
 * Notification that a message has been sent
 * (does nothing)
 */
void CWapProtocolObserver::MessageSendCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_MESSAGESENDCOMPLETED_1, "CWapProtocolObserver::MessageSendCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Informs protocol whether client confirms received message
 */
TBool CWapProtocolObserver::ClientConfirmsMessage()const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_CLIENTCONFIRMSMESSAGE_1, "CWapProtocolObserver::ClientConfirmsMessage()");

	return EFalse;
	} // CWapProtocolObserver::ClientConfirmsMessage


/**
 *  Informs protocol whether client confirms received message
 */
TInt CWapProtocolObserver::SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver,const TSmsAddr& aAddr)const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_SMSADDRISDUPLICATE_1, "CWapProtocolObserver::SmsAddrIsDuplicate()");

	if(this == aObserver)return EFalse;
	if(aAddr.SmsAddrFamily() == ESmsAddrApplication8BitPort || ( aAddr.SmsAddrFamily() == ESmsAddrApplication16BitPort && aAddr.Port() > 255))
		{
		TWapAddr wapAddr;
		wapAddr.SetPort(aAddr.Port());
		if(iWapSmsProtocol->AddrAlreadyUsedByWAP(wapAddr,NULL) != KErrNone) return ETrue;
		else return EFalse;
		}
	return iSmsAddr==aAddr;
	} // CWapProtocolObserver::SmsAddrIsDuplicate


/**
 *  Notification that a message has been received
 */
TInt CWapProtocolObserver::MessageReceived(const CSmsMessage& aSmsMessage,TDes& /*aDes*/)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_MESSAGERECEIVED_1, "CWapProtocolObserver::MessageReceived");

	TRAPD(ret, iWapSmsProtocol->ProcessSmsL(aSmsMessage) );
	return ret;
	} // CWapProtocolObserver::MessageReceived


/**
 *  Notification that the enumeration has completed
 *  (Has no relevance for the wap stack)
 */
void CWapProtocolObserver::EnumeratePhoneCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_ENUMERATEPHONECOMPLETED_1, "CWapProtocolObserver::EnumeratePhoneCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a message has been written to the phone or SIM
 *  (Has no relevance for the wap stack)
 */
void CWapProtocolObserver::MessageWriteCompleted(TInt /*aStatus*/, const CSmsMessage* /*aSmsMessage*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_MESSAGEWRITECOMPLETED_1, "CWapProtocolObserver::MessageWriteCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a PDU delete has completed
 *  (Has no relevance for the wap stack)
 */
void CWapProtocolObserver::MessageDeleteCompleted(TInt /*aStatus*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_MESSAGEDELETECOMPLETED_1, "CWapProtocolObserver::MessageDeleteCompleted()");
    BULLSEYE_RESTORE
    }

/**
 *  Notification that a read SMS parameters has completed
 *  (Has no relevance for the wap stack)
 */
void CWapProtocolObserver::ReadSmsParamsCompleted(TInt /*aStatus*/,CMobilePhoneSmspList* /*aSmspList*/)
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_READSMSPARAMSCOMPLETED_1, "CWapProtocolObserver::ReadSmsParamsCompleted()");
    BULLSEYE_RESTORE
    }

void CWapProtocolObserver::WriteSmsParamsCompleted(TInt /*aStatus*/)
/**
 *  Notification that a write SMS parameters has completed
 *  (Has no relevance for the wap stack)
 */
    {
    // Ignore in code coverage - implementation required by base class
    // but not relavent for WAP PROT.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPPROTOCOLOBSERVER_WRITESMSPARAMSCOMPLETED_1, "CWapProtocolObserver::WriteSmsParamsCompleted()");
    BULLSEYE_RESTORE
    }
