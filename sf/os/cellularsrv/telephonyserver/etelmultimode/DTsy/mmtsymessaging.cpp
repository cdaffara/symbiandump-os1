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

#include "mmtsy.h"
#include "testdef.h"
#include "ETELMM.H"
#include <et_clsvr.h>
#include "Dmmlog.h"

/**************************************************************************/
//
//	CSmsMessagingDMmTsy
//
/**************************************************************************/

CSmsMessagingDMmTsy* CSmsMessagingDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CSmsMessagingDMmTsy* subsession=new(ELeave) CSmsMessagingDMmTsy(aPhone, aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CSmsMessagingDMmTsy::Init()
	{}

CSmsMessagingDMmTsy::CSmsMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iMessagingName=aName;
	}

void CSmsMessagingDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy created"));
	iSmspReadAll = new (ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);
	}

CSmsMessagingDMmTsy::~CSmsMessagingDMmTsy()
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy destroyed"));
	if (iSmspReadAll)
		iSmspReadAll->ResetAndDestroy();
	delete iSmspReadAll;
	}

CTelObject* CSmsMessagingDMmTsy::OpenNewObjectByNameL(const TDesC& aName)
	{
	// SMS related stores can be opened from an SMS messaging sub-session
	// Clients must supply the name of the store they wish to open

	if (aName.Compare(KETelMeSmsStore)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CSmsStoreDMmTsy::NewL(iPhone,FacPtr(),aName));
	
	if (aName.Compare(KETelIccSmsStore)==KErrNone)
		return REINTERPRET_CAST(CTelObject*, CSmsStoreDMmTsy::NewL(iPhone,FacPtr(),aName));

	return NULL;
	}

CTelObject* CSmsMessagingDMmTsy::OpenNewObjectL(TDes&)
	{
	// Open objects without a name is not supported
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CSmsMessagingDMmTsy::ReqModeL(const TInt aIpc)
	{
	// ReqModeL is called from the server's CTelObject::ReqAnalyserL
	// in order to check the type of request it has

	// The following are example request types for this dummy TSY
	// All TSYs do not have to have these request types but they have been given
	// "sensible" values in this test code

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{

//
// Non-flow Controlled Services
//
	case EMobileSmsMessagingGetCaps:
	case EMobileSmsMessagingGetReceiveMode:
	case EMobileSmsMessagingGetMoSmsBearer:
	case EMobileSmsMessagingEnumerateMessageStores:
	case EMobileSmsMessagingGetMessageStoreInfo:
	case EMobileSmsMessagingResumeSmsReception:
	case EMobileSmsMessagingGetSmspListPhase2:
		break;

//
// Flow Controlled Services
//
	case EMobileSmsMessagingSetReceiveMode:
	case EMobileSmsMessagingSetMoSmsBearer:
	case EMobileSmsMessagingAckSmsStored:
	case EMobileSmsMessagingNackSmsStored:
	case EMobileSmsMessagingSendMessage:
	case EMobileSmsMessagingSendMessageNoFdnCheck:
	case EMobileSmsMessagingGetSmspListPhase1:
	case EMobileSmsMessagingStoreSmspList:
		ret=KReqModeFlowControlObeyed;
		break;

//
//  Immediate server repost of request
//
	case EMobileSmsMessagingReceiveMessage:
		ret=KReqModeRePostImmediately;
		break;
//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileSmsMessagingNotifyReceiveModeChange:
	case EMobileSmsMessagingNotifyMoSmsBearerChange:
	case EMobileSmsMessagingNotifySmspListChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;

//
// Cancel Requests
//	
	case EMobileSmsMessagingSetReceiveModeCancel:
	case EMobileSmsMessagingNotifyReceiveModeChangeCancel:
	case EMobileSmsMessagingNotifyMoSmsBearerChangeCancel:
	case EMobileSmsMessagingAckSmsStoredCancel:
	case EMobileSmsMessagingNackSmsStoredCancel:
	case EMobileSmsMessagingSendMessageCancel:
	case EMobileSmsMessagingSendMessageNoFdnCheckCancel:
	case EMobileSmsMessagingGetMessageStoreInfoCancel:
	case EMobileSmsMessagingGetSmspListCancel:
	case EMobileSmsMessagingStoreSmspListCancel:
	case EMobileSmsMessagingNotifySmspListChangeCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CSmsMessagingDMmTsy::RegisterNotification(const TInt aIpc)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileSmsMessagingNotifySmspListChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: RegisterNotification - SMSP List Change"));
		return KErrNone;
	case EMobileSmsMessagingNotifyReceiveModeChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: RegisterNotification - SMS Receive Mode Change"));
		return KErrNone;
	case EMobileSmsMessagingNotifyMoSmsBearerChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: RegisterNotification - SMS Bearer Change"));
		return KErrNone;
	case EMobileSmsMessagingReceiveMessage:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: RegisterNotification - Receive Message"));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CSmsMessagingDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CSmsMessagingDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may 
	// receive from the phone

	switch (aIpc)
		{
	case EMobileSmsMessagingNotifySmspListChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: DeregisterNotification - SMSP List Change"));
		return KErrNone;
	case EMobileSmsMessagingNotifyReceiveModeChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: DeregisterNotification - SMS Receive Mode Change"));
		return KErrNone;
	case EMobileSmsMessagingNotifyMoSmsBearerChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: DeregisterNotification - SMS Bearer Change"));
		return KErrNone;
	case EMobileSmsMessagingReceiveMessage:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: DeregisterNotification - Receive Message"));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CSmsMessagingDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CSmsMessagingDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileSmsMessagingNotifySmspListChange:
	case EMobileSmsMessagingNotifyReceiveModeChange:
	case EMobileSmsMessagingNotifyMoSmsBearerChange:
	case EMobileSmsMessagingReceiveMessage:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CSmsMessagingDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CSmsMessagingDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY

	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobileSmsMessagingGetCaps:
		return GetCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileSmsMessagingEnumerateMessageStores:
		return EnumerateMessageStores(aTsyReqHandle, 
			REINTERPRET_CAST(TInt*, dataPtr));

	case EMobileSmsMessagingGetMessageStoreInfo:
		return GetMessageStoreInfo(aTsyReqHandle, 
			REINTERPRET_CAST(TInt*, dataPtr),
			aPackage.Des2n());

	case EMobileSmsMessagingReceiveMessage:
		return ReceiveMessage(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());

	case EMobileSmsMessagingAckSmsStored:
		return AckSmsStored(aTsyReqHandle, aPackage.Des1n(),
			REINTERPRET_CAST(TBool*, dataPtr2));

	case EMobileSmsMessagingNackSmsStored:
		return NackSmsStored(aTsyReqHandle, aPackage.Des1n(),
			REINTERPRET_CAST(TInt*, dataPtr2));

	case EMobileSmsMessagingResumeSmsReception:
		return ResumeSmsReception(aTsyReqHandle);

	case EMobileSmsMessagingSendMessage:
		return SendMessage(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());

	case EMobileSmsMessagingSendMessageNoFdnCheck:
		return SendMessageNoFdnCheck(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());

	case EMobileSmsMessagingGetSmspListPhase1:
		return ReadSmspListPhase1(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr),
			REINTERPRET_CAST(TInt*,dataPtr2));

	case EMobileSmsMessagingGetSmspListPhase2:
		return ReadSmspListPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*,dataPtr), aPackage.Des2n());

	case EMobileSmsMessagingStoreSmspList:
		return StoreSmspList(aTsyReqHandle, aPackage.Des1n());

	case EMobileSmsMessagingNotifySmspListChange:
		return NotifySmspListChange(aTsyReqHandle);

	case EMobileSmsMessagingGetReceiveMode:
		return GetReceiveMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsReceiveMode*, dataPtr));

	case EMobileSmsMessagingSetReceiveMode:
		return SetReceiveMode(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsReceiveMode*, dataPtr));

	case EMobileSmsMessagingNotifyReceiveModeChange:
		return NotifyReceiveModeChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsReceiveMode*, dataPtr));

	case EMobileSmsMessagingGetMoSmsBearer:
		return GetMoSmsBearer(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsBearer*, dataPtr));

	case EMobileSmsMessagingSetMoSmsBearer:
		return SetMoSmsBearer(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsBearer*, dataPtr));

	case EMobileSmsMessagingNotifyMoSmsBearerChange:
		return NotifyMoSmsBearerChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsBearer*, dataPtr));

	default:
		return KErrNotSupported;
		}
	}

TInt CSmsMessagingDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.

	LOGTEXT(_L8("CSmsMessagingDMmTsy: - CancelService called"));
	switch (aIpc)
		{
	case EMobileSmsMessagingReceiveMessage:
		return ReceiveMessageCancel(aTsyReqHandle);
	case EMobileSmsMessagingAckSmsStored:
		return AckSmsStoredCancel(aTsyReqHandle);
	case EMobileSmsMessagingNackSmsStored:
		return NackSmsStoredCancel(aTsyReqHandle);
	case EMobileSmsMessagingSendMessage:
		return SendMessageCancel(aTsyReqHandle);
	case EMobileSmsMessagingSendMessageNoFdnCheck:
		return SendMessageNoFdnCheckCancel(aTsyReqHandle);
	case EMobileSmsMessagingGetMessageStoreInfo:
		return GetMessageStoreInfoCancel(aTsyReqHandle);
	case EMobileSmsMessagingGetSmspListPhase1:
	case EMobileSmsMessagingGetSmspListPhase2:
		return ReadSmspListCancel(aTsyReqHandle);
	case EMobileSmsMessagingStoreSmspList:
		return StoreSmspListCancel(aTsyReqHandle);
	case EMobileSmsMessagingNotifySmspListChange:
		return NotifySmspListChangeCancel(aTsyReqHandle);
	case EMobileSmsMessagingSetReceiveMode:
		return SetReceiveModeCancel(aTsyReqHandle);
	case EMobileSmsMessagingNotifyReceiveModeChange:
		return NotifyReceiveModeChangeCancel(aTsyReqHandle);
	case EMobileSmsMessagingSetMoSmsBearer:
		return SetMoSmsBearerCancel(aTsyReqHandle);
	case EMobileSmsMessagingNotifyMoSmsBearerChange:
		return NotifyMoSmsBearerChangeCancel(aTsyReqHandle);
	default:
		return KErrGeneral; 
		}
	}


/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/


TInt CSmsMessagingDMmTsy::GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::GetCaps called"));
	RMobileSmsMessaging::TMobileSmsCapsV1Pckg* smsCapsPckg = (RMobileSmsMessaging::TMobileSmsCapsV1Pckg*)aCaps;
	RMobileSmsMessaging::TMobileSmsCapsV1& smsCaps = (*smsCapsPckg)();

	smsCaps.iSmsMode=DMMTSY_SMS_MESSAGING_MODE_CAPS;
	smsCaps.iSmsControl=DMMTSY_SMS_MESSAGING_CONTROL_CAPS;

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsg, TDes8* aMsgAttributes)
	{
	if (!iReceiveMessage++)
		{
		LOGTEXT(_L8("CSmsMessagingDMmTsy::ReceiveMessage called"));
	
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg*, aMsgAttributes);
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& atts = (*attsPckg)();

		atts.iFlags=DMMTSY_SMS_MESSAGE_FLAGS1;
		atts.iDataFormat=DMMTSY_SMS_MESSAGE_FORMAT1;
		atts.iGsmServiceCentre.iTelNumber=DMMTSY_SMS_MESSAGE_SC1;
		atts.iOriginator.iTelNumber=DMMTSY_SMS_MESSAGE_REMOTEPARTYNUMBER1;
		atts.iStatus=DMMTSY_SMS_MESSAGE_STATUS1;
		atts.iStore=DMMTSY_MESSAGE_STORE_NAME;
		atts.iStoreIndex=DMMTSY_SMS_STORE_INDEX1;

		aMsg->Copy(DMMTSY_SMS_MESSAGE_PDU1);
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::ReceiveMessageCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::AckSmsStored(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsg, TBool* aFull)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::AckSmsStored called"));

	if (((aMsg->Length()!=0) && (aMsg->Compare(DMMTSY_SMS_MESSAGE_PDU1)!=0)) || 
		(*aFull != DMMTSY_SMS_MESSAGE_FULL))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::AckSmsStoredCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::AckSmsStoredCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NackSmsStored(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TInt* aRpCause)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::NackSmsStored called"));
	if ((aMsgData->Compare(DMMTSY_SMS_MESSAGE_PDU1) != 0 ) || *aRpCause != DMMTSY_SMS_MESSAGE_NACK_CAUSE)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NackSmsStoredCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::NackSmsStoredCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::ResumeSmsReception(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::ResumeSmsReception called"));
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SendMessage(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsg, TDes8* aMsgAttributes)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SendMessage called"));

	RMobileSmsMessaging::TMobileSmsAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsAttributesV1Pckg*, aMsgAttributes);
	RMobileSmsMessaging::TMobileSmsAttributesV1& atts = (*attsPckg)();
	
	if (atts.ExtensionId() == RMobileSmsMessaging::KETelMobileSmsSendAttributesV1)
		{
		RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* attsV1Pckg = REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg*, aMsgAttributes);
		RMobileSmsMessaging::TMobileSmsSendAttributesV1& attsV1 = (*attsV1Pckg)();

		if ((attsV1.iFlags != DMMTSY_SMS_MESSAGE_FLAGS2) ||
			(attsV1.iDataFormat != DMMTSY_SMS_MESSAGE_FORMAT1) ||
			(attsV1.iDestination.iTelNumber != DMMTSY_PHONE_TEL_NUMBER) ||
			(attsV1.iMore != DMMTSY_SMS_MESSAGE_MORE) ||
			(aMsg->Compare(DMMTSY_SMS_MESSAGE_PDU1) != 0))
			{
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
			}
		else
			{
			attsV1.iMsgRef = DMMTSY_SMS_MESSAGE_MSGREF;
			attsV1.iSubmitReport = DMMTSY_SMS_MESSAGE_PDU2;
			iPhone->AddDelayedReq(aTsyReqHandle, this);
			}
		}
	else if (atts.ExtensionId() == RMobileSmsMessaging::KETelMobileSmsCdmaSendAttributesV4)
		{
		RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4Pckg* attsV4Pckg = REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4Pckg*, aMsgAttributes);
		RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4& attsV4 = (*attsV4Pckg)();

		if ((attsV4.iFlags != DMMTSY_SMS_MESSAGE_CDMA_FLAGS) ||
			(attsV4.iDataFormat != DMMTSY_SMS_MESSAGE_CDMA_FORMAT) ||
			(attsV4.iCdmaTeleservice != DMMTSY_SMS_MESSAGE_CDMA_TELESERVICE) ||
			(attsV4.iCdmaServiceCategory != DMMTSY_SMS_MESSAGE_CDMA_SERVICE_CATEGORY) ||
			(attsV4.iMore != DMMTSY_SMS_MESSAGE_MORE) ||
			(attsV4.iDestination.iTypeOfNumber != DMMTSY_SMS_MESSAGE_TON) ||
			(attsV4.iDestination.iNumberPlan != DMMTSY_SMS_MESSAGE_NP) ||
			(attsV4.iDestination.iTelNumber != DMMTSY_PHONE_TEL_NUMBER) ||
			(attsV4.iAddressAuthentication != DMMTSY_SMS_MESSAGE_ADDRESS_AUTHENTICATION) ||
			(aMsg->Compare(DMMTSY_SMS_MESSAGE_PDU1) != 0))
			{
			ReqCompleted(aTsyReqHandle, KErrCorrupt);
			}
		else
			{
			attsV4.iMsgRef = DMMTSY_SMS_MESSAGE_MSGREF;
			attsV4.iTLAck = DMMTSY_SMS_MESSAGE_TLACK;
			iPhone->AddDelayedReq(aTsyReqHandle, this);
			}
		}
	else
		{
		ReqCompleted(aTsyReqHandle, KErrCorrupt);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SendMessageCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SendMessageCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SendMessageNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsg, TDes8* aMsgAttributes)
	/**
	* This method sends an outgoing SMS to the network. The number used for sending the SMS   
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*
	* @param aMsg Supplies the TPDU to send
	* @param aMsgAttributes Supplies the attributes of the outgoing message
	*/
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SendMessageNoFdnCheck called"));
	
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg*, aMsgAttributes);
	RMobileSmsMessaging::TMobileSmsSendAttributesV1& atts = (*attsPckg)();

	if ((atts.iFlags != DMMTSY_SMS_MESSAGE_FLAGS2) ||
		(atts.iDataFormat != DMMTSY_SMS_MESSAGE_FORMAT1) ||
		(atts.iDestination.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) != 0) ||
		(atts.iMore != DMMTSY_SMS_MESSAGE_MORE) ||
		(aMsg->Compare(DMMTSY_SMS_MESSAGE_PDU1) != 0))
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		atts.iMsgRef=DMMTSY_SMS_MESSAGE_MSGREF;
		atts.iSubmitReport=DMMTSY_SMS_MESSAGE_PDU2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SendMessageNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle)
	/**
	* This method cancels the sending of an outgoing SMS to the network. The number used for sending the SMS   
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*
	*/
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SendMessageNoFdnCheckCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::EnumerateMessageStores(const TTsyReqHandle aTsyReqHandle, TInt* aCount)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::EnumerateMessageStores called"));
	*aCount=DMMTSY_PHONE_MESSAGE_STORE_COUNT;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::GetMessageStoreInfo(const TTsyReqHandle aTsyReqHandle, TInt *aIndex, TDes8* aInfo)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::GetMessageStoreInfo called"));
	
	if (*aIndex != DMMTSY_SMS_STORE_INDEX1)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg* infoPckg = REINTERPRET_CAST(RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*, aInfo);
		RMobilePhoneStore::TMobilePhoneStoreInfoV1& info = (*infoPckg)();

		info.iName=DMMTSY_MESSAGE_STORE_NAME;
		info.iType=DMMTSY_MESSAGE_STORE_TYPE;
		info.iTotalEntries=DMMTSY_MESSAGE_STORE_TOTAL_ENTRIES;
		info.iUsedEntries=DMMTSY_MESSAGE_STORE_USED_ENTRIES;
		info.iCaps=DMMTSY_MESSAGE_STORE_CAPS;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::GetMessageStoreInfoCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::GetMessageStoreInfoCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::ValidateSmspEntry(const RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
	{
	if ((aEntry.iText != DMMTSY_SMSP_STORE_TEXT1) ||
		(aEntry.iValidParams != DMMTSY_SMSP_STORE_VALID_PARAMS1) ||
		(aEntry.iDestination.iTelNumber.Compare(DMMTSY_PHONE_TEL_NUMBER) != 0) ||
		(aEntry.iValidityPeriod != DMMTSY_SMSP_STORE_VALIDITY1) ||
		(aEntry.iServiceCentre.iTelNumber.Compare(DMMTSY_SMSP_STORE_SCA) != 0) ||
		(aEntry.iProtocolId != DMMTSY_SMSP_STORE_PID1) ||
		(aEntry.iDcs != DMMTSY_SMSP_STORE_DCS1))
		return KErrCorrupt;
	else
		return KErrNone;
	}

void CSmsMessagingDMmTsy::InitSmspEntry(RMobileSmsMessaging::TMobileSmspEntryV1& aEntry)
	{
	aEntry.iText = DMMTSY_SMSP_STORE_TEXT1;
	aEntry.iValidParams = DMMTSY_SMSP_STORE_VALID_PARAMS1;
	aEntry.iDestination.iTelNumber = DMMTSY_PHONE_TEL_NUMBER;
	aEntry.iValidityPeriod = DMMTSY_SMSP_STORE_VALIDITY1;
	aEntry.iServiceCentre.iTelNumber = DMMTSY_SMSP_STORE_SCA;
	aEntry.iProtocolId = DMMTSY_SMSP_STORE_PID1;
	aEntry.iDcs = DMMTSY_SMSP_STORE_DCS1;
	}


TInt CSmsMessagingDMmTsy::ReadSmspListPhase1(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TInt* aBufSize)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::ReadSmspListPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessReadSmspListPhase1L(aTsyReqHandle, aId, aBufSize););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CSmsMessagingDMmTsy::ProcessReadSmspListPhase1L(const TTsyReqHandle aTsyReqHandle, 
											 RMobilePhone::TClientId* aId, 
											 TInt* aBufSize)
	{
	// read the list, store its content and then return size of this buffer to client
	CMobilePhoneSmspList* list=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(list);

	RMobileSmsMessaging::TMobileSmspEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_SMSP_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			InitSmspEntry(entry);

			entry.iIndex=DMMTSY_SMSP_STORE_INDEX1;
			break;
		case 1:
			InitSmspEntry(entry);

			entry.iIndex=DMMTSY_SMSP_STORE_INDEX2;
		default:
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aId,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC
	
	iSmspReadAll->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}


TInt CSmsMessagingDMmTsy::ReadSmspListPhase2(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aId, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::ReadSmspListPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iSmspReadAll->Count(); ++i)
		{
		read = iSmspReadAll->At(i);
		if ((read->iClient.iSessionHandle==aId->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aId->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuffer->Copy(bufPtr);
			delete read;
			iSmspReadAll->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	return KErrNotFound;
	}


TInt CSmsMessagingDMmTsy::ReadSmspListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::ReadSmspListCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	
	// Remove the read all attempt from iSmspReadAll
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iSmspReadAll->Count(); ++i)
		{
		read = iSmspReadAll->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iSmspReadAll->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::StoreSmspList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::StoreSmspList called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessStoreSmspListL(aTsyReqHandle, aBuffer););
	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	};

TInt CSmsMessagingDMmTsy::ProcessStoreSmspListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneSmspList* aSmspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(aSmspList);
	
	aSmspList->RestoreL(*aBuffer);

	RMobileSmsMessaging::TMobileSmspEntryV1 entry;

	if (aSmspList->Enumerate() != DMMTSY_SMSP_STORE_COUNT)
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aSmspList->GetEntryL(0);

	if ((entry.iIndex != DMMTSY_SMSP_STORE_INDEX1) ||  
		(ValidateSmspEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	entry=aSmspList->GetEntryL(1);

	if ((entry.iIndex != DMMTSY_SMSP_STORE_INDEX2) ||  
		(ValidateSmspEntry(entry) != KErrNone))
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		return KErrNone;
		}

	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // aSmspList
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::StoreSmspListCancel(TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmspStoreDMmTsy::StoreSmspListCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifySmspListChange(const TTsyReqHandle aTsyReqHandle)
	{
	if (!iNotifySmspListChange++)
		{
		LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifySmspListChange called"));
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifySmspListChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifySmspListChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}


TInt CSmsMessagingDMmTsy::GetReceiveMode(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::GetReceiveMode called"));
	*aReceiveMode=DMMTSY_SMS_MESSAGING_RECEIVE_MODE1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SetReceiveMode(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SetReceiveMode called"));
	if ( *aReceiveMode != DMMTSY_SMS_MESSAGING_RECEIVE_MODE1 )
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SetReceiveModeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SetReceiveModeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifyReceiveModeChange(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode)
	{
	if (!iNotifyReceiveModeChange++)
		{
		LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifyReceiveModeChange called"));
		*aReceiveMode=DMMTSY_SMS_MESSAGING_RECEIVE_MODE2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifyReceiveModeChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifyReceiveModeChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::GetMoSmsBearer(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::GetMoSmsBearer called"));
	*aBearer=DMMTSY_SMS_MESSAGING_BEARER1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SetMoSmsBearer(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SetMoSmsBearer called"));
	if ( *aBearer != DMMTSY_SMS_MESSAGING_BEARER1 )
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::SetMoSmsBearerCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::SetMoSmsBearerCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifyMoSmsBearerChange(const TTsyReqHandle aTsyReqHandle, RMobileSmsMessaging::TMobileSmsBearer* aBearer)
	{
	if (!iNotifyMoSmsBearerChange++)
		{
		LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifyMoSmsBearerChange called"));
		*aBearer=DMMTSY_SMS_MESSAGING_BEARER2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CSmsMessagingDMmTsy::NotifyMoSmsBearerChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CSmsMessagingDMmTsy::NotifyMoSmsBearerChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

/**************************************************************************/
//
//	CBroadcastMessagingDMmTsy
//
/**************************************************************************/

CBroadcastMessagingDMmTsy* CBroadcastMessagingDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CBroadcastMessagingDMmTsy* subsession=new(ELeave) CBroadcastMessagingDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CBroadcastMessagingDMmTsy::Init()
	{}

CBroadcastMessagingDMmTsy::CBroadcastMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone), iCbmiListRequests(NULL)
	{
	iMessagingName=aName;
	}

void CBroadcastMessagingDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy created"));
	iCbmiListRequests = new (ELeave) CArrayPtrFlat<CListReadAllAttempt>(16);
	}

CBroadcastMessagingDMmTsy::~CBroadcastMessagingDMmTsy()
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy destroyed"));
	iCbmiListRequests->ResetAndDestroy();
	delete iCbmiListRequests;
	}

CTelObject* CBroadcastMessagingDMmTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	// Broadcast related stores can be opened from a Broadcast messaging sub-session


	return NULL;
	}

CTelObject* CBroadcastMessagingDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CBroadcastMessagingDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Non-Flow Controlled Services
//
	case EMobileBroadcastMessagingGetFilterSetting:
	case EMobileBroadcastMessagingGetCaps:
	case EMobileBroadcastMessagingGetLanguageFilter:
	case EMobileBroadcastMessagingSetLanguageFilter:
	case EMobileBroadcastMessagingGetIdListPhase1:
	case EMobileBroadcastMessagingGetIdListPhase2:
	case EMobileBroadcastMessagingStoreIdList:
		break;

//
// Flow Controlled Services
//
	case EMobileBroadcastMessagingSetFilterSetting:
		ret=KReqModeFlowControlObeyed;
		break;

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileBroadcastMessagingReceiveMessage:
	case EMobileBroadcastMessagingNotifyFilterSettingChange:
	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
	case EMobileBroadcastMessagingNotifyIdListChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//	
	case EMobileBroadcastMessagingReceiveMessageCancel:
	case EMobileBroadcastMessagingNotifyFilterSettingChangeCancel:
	case EMobileBroadcastMessagingSetFilterSettingCancel:
	case EMobileBroadcastMessagingGetLanguageFilterCancel:
	case EMobileBroadcastMessagingSetLanguageFilterCancel:
	case EMobileBroadcastMessagingNotifyLanguageFilterChangeCancel:
	case EMobileBroadcastMessagingGetIdListCancel:
	case EMobileBroadcastMessagingStoreIdListCancel:
	case EMobileBroadcastMessagingNotifyIdListChangeCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CBroadcastMessagingDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileBroadcastMessagingReceiveMessage:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: RegisterNotification - Receive Message"));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyFilterSettingChange:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: RegisterNotification - Receive setting Change "));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: RegisterNotification - Language Filter Change "));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyIdListChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: RegisterNotification - Notify BroadcastId List Change"));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CBroadcastMessagingDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileBroadcastMessagingReceiveMessage:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: DeregisterNotification - Receive Message"));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyFilterSettingChange:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: DeregisterNotification - Receive setting Change "));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: DeregisterNotification - Language Filter Change "));
		return KErrNone;
	case EMobileBroadcastMessagingNotifyIdListChange:
		LOGTEXT(_L8("CSmsMessagingDMmTsy: DeregisterNotification - Notify BroadcastId List Change"));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CBroadcastMessagingDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileBroadcastMessagingReceiveMessage:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: Registered with 10 slots"));
		numberOfSlots=10;
		break;
	case EMobileBroadcastMessagingNotifyFilterSettingChange:
	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
	case EMobileBroadcastMessagingNotifyIdListChange:
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: Registered with 2 slots"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CBroadcastMessagingDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	TInt err;
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobileBroadcastMessagingGetCaps:
		return GetCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileBroadcastMessagingGetFilterSetting:
		return GetFilterSetting(aTsyReqHandle,
			REINTERPRET_CAST(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter*, dataPtr));
	
	case EMobileBroadcastMessagingSetFilterSetting:
		return SetFilterSetting(aTsyReqHandle,
			REINTERPRET_CAST(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter*, dataPtr));

	case EMobileBroadcastMessagingNotifyFilterSettingChange:
		return NotifyFilterSettingChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter*, dataPtr));
	
	case EMobileBroadcastMessagingReceiveMessage:
		return ReceiveMessage(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());
	
	case EMobileBroadcastMessagingGetLanguageFilter:
		return GetLanguageFilter(aTsyReqHandle, aPackage.Des1u());

	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
		return NotifyLanguageFilterChange(aTsyReqHandle, aPackage.Des1u());

	case EMobileBroadcastMessagingSetLanguageFilter:
		return SetLanguageFilter(aTsyReqHandle, aPackage.Des1u());
	
	case EMobileBroadcastMessagingReceiveMessageCancel:
		return ReceiveMessageCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingNotifyFilterSettingChangeCancel:
		return NotifyFilterSettingChangeCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingSetFilterSettingCancel:
		return SetFilterSettingCancel(aTsyReqHandle);

	case EMobileBroadcastMessagingGetIdListPhase1:
		return GetBroadcastIdListPhase1(aTsyReqHandle,
			REINTERPRET_CAST(CRetrieveMobilePhoneBroadcastIdList::TGetBroadcastIdRequest*, dataPtr),
			REINTERPRET_CAST(TInt*, dataPtr2));
	case EMobileBroadcastMessagingGetIdListPhase2:
		return GetBroadcastIdListPhase2(aTsyReqHandle,
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr),
			aPackage.Des2n());
	case EMobileBroadcastMessagingStoreIdList:
		{
		TRAP(err,StoreBroadcastIdListL(aTsyReqHandle,aPackage.Des1n()));
		if (err != KErrNone)
			ReqCompleted(aTsyReqHandle,err);
		return KErrNone;
		}
	case EMobileBroadcastMessagingNotifyIdListChange:
		return NotifyBroadcastIdListChange(aTsyReqHandle);
	default:
		return KErrNotSupported;
		}
	}

TInt CBroadcastMessagingDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy: - CancelService called"));
	switch (aIpc)
		{
	case EMobileBroadcastMessagingReceiveMessage:
		return ReceiveMessageCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingNotifyFilterSettingChange:
		return NotifyFilterSettingChangeCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingSetFilterSetting:
		return SetFilterSettingCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingGetLanguageFilter:
		return GetLanguageFilterCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingSetLanguageFilter:
		return SetLanguageFilterCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingNotifyLanguageFilterChange:
		return NotifyLanguageFilterChangeCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingGetIdListPhase1:
	case EMobileBroadcastMessagingGetIdListPhase2:
		return GetBroadcastIdListCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingStoreIdList:
		return StoreBroadcastIdListCancel(aTsyReqHandle);
	case EMobileBroadcastMessagingNotifyIdListChange:
		return NotifyBroadcastIdListChangeCancel(aTsyReqHandle);
	default:
		return KErrGeneral; 
		}
	}


TInt CBroadcastMessagingDMmTsy::GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetCaps called"));
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg* cbsCapsPckg = (RMobileBroadcastMessaging::TMobileBroadcastCapsV1Pckg*)aCaps;
	RMobileBroadcastMessaging::TMobileBroadcastCapsV1& cbsCaps = (*cbsCapsPckg)();

	cbsCaps.iModeCaps=DMMTSY_BROADCAST_MESSAGING_MODE_CAPS;
	cbsCaps.iFilterCaps=DMMTSY_BROADCAST_MESSAGING_FILTER_CAPS;

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgData, TDes8* aMsgAtts)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::ReceiveMessage called"));

	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileBroadcastMessaging::TMobileBroadcastAttributesV1Pckg*, aMsgAtts);
	RMobileBroadcastMessaging::TMobileBroadcastAttributesV1 &atts = (*attsPckg)();
	
	if(atts.ExtensionId()==KETelExtMultimodeV2)
		{
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy::ReceiveMessage is using V2 attributes"));

		RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg* attsV2Pckg = REINTERPRET_CAST(RMobileBroadcastMessaging::TMobileBroadcastAttributesV2Pckg*, aMsgAtts);
		RMobileBroadcastMessaging::TMobileBroadcastAttributesV2 &attsV2 = (*attsV2Pckg)();

		attsV2.iFormat = DMMTSY_CBS_MESSAGE_PDU_FORMAT;
		attsV2.iFlags = DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS;
		attsV2.iDCS = DMMTSY_BROADCAST_MESSAGING_ATT_DCS;
		attsV2.iMessageId = DMMTSY_BROADCAST_MESSAGING_ATT_MSGID;
		attsV2.iMessageType =DMMTSY_BROADCAST_MESSAGING_ATT_MSGTYPE;
		attsV2.iNumberOfPages = DMMTSY_BROADCAST_MESSAGING_ATT_NUMOFPAGES;
		attsV2.iSerialNum = DMMTSY_BROADCAST_MESSAGING_ATT_SERIAL;

		*aMsgData = DMMTSY_CBS_MESSAGE_PDU;
		}
	else if(atts.ExtensionId()==KETelExtMultimodeV1)
		{
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy::ReceiveMessage is using V1 attributes"));

		atts.iFormat = DMMTSY_CBS_MESSAGE_PDU_FORMAT;
		atts.iFlags = DMMTSY_BROADCAST_MESSAGING_ATT_FLAGS;
		*aMsgData = DMMTSY_CBS_MESSAGE_PDU;
		}
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::ReceiveMessageCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::GetFilterSetting(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetFilterSetting called"));
	*aSetting=DMMTSY_BROADCAST_RECEIVE_SETTING1;
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::SetFilterSetting(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::SetFilterSetting called"));
	if ( *aSetting != DMMTSY_BROADCAST_RECEIVE_SETTING2 )
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::SetFilterSettingCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::SetFilterSettingCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyFilterSettingChange(const TTsyReqHandle aTsyReqHandle, RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting)
	{
	if (!iNotifyFilterSettingChange++)
		{
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyFilterSettingChange called"));
		*aSetting=DMMTSY_BROADCAST_RECEIVE_SETTING2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyFilterSettingChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyFilterSettingChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::GetLanguageFilter(const TTsyReqHandle aTsyReqHandle, TDes16* aLangFilter)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetLanguageFilter called"));
	*aLangFilter = DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER;
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::GetLanguageFilterCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetLanguageFilterCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::SetLanguageFilter(const TTsyReqHandle aTsyReqHandle, TDesC16* aLangFilter)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::SetLanguageFilter called"));
	if (aLangFilter->Compare(DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER) != 0)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::SetLanguageFilterCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::SetLanguageFilterCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyLanguageFilterChange(const TTsyReqHandle aTsyReqHandle, TDes16* aLangFilter)
	{
	if (!iNotifyLanguageFilterChange++)
		{
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyLanguageFilterChange called"));
		*aLangFilter = DMMTSY_CBS_MESSAGE_LANGUAGE_FILTER;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyLanguageFilterChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyLanguageFilterChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::GetBroadcastIdListPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneBroadcastIdList::TGetBroadcastIdRequest* aReqData, TInt* aBufSize)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetBroadcastIdListPhase1 called"));
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;

	if (aReqData->iIdType != DMMTSY_BROADCAST_ID_TYPE)
		leaveCode = KErrCorrupt;

	if (!leaveCode)
		TRAP(leaveCode, ret=ProcessGetBroadcastIdListPhase1L(aTsyReqHandle, &aReqData->iClient, aBufSize););

	if (leaveCode != KErrNone)
		ReqCompleted(aTsyReqHandle,leaveCode);
	return ret;
	}

TInt CBroadcastMessagingDMmTsy::ProcessGetBroadcastIdListPhase1L(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSize)
	{
	CMobilePhoneBroadcastIdList* list=CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);

	RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1 entry;

	// fill up an example list
	for (TInt index=0; index < DMMTSY_CBMI_STORE_COUNT; index++)
		{
		switch (index)
			{
		case 0:
			entry.iId=DMMTSY_CBMI_ID1;
			break;
		case 1:
			entry.iId=DMMTSY_CBMI_ID2;
			break;
		case 2:
			entry.iId=DMMTSY_CBMI_ID3;
			break;
			}
			// Add the entry into the list, at the next empty location
			list->AddEntryL(entry);
		}

	list->SetMaxNumberEntries(DMMTSY_CBMI_MAX_ENTRIES);

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC
	
	iCbmiListRequests->AppendL(read);
	CleanupStack::Pop(); // pop the CListReadAllAttempt
	
	// return the CBufFlat’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	// Complete first phase of list retrieval
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	CleanupStack::PopAndDestroy(); // pop&destroy list
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::GetBroadcastIdListPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetBroadcastIdListPhase2 called"));
	CListReadAllAttempt* read=NULL;
	// Find the read attempt from this client
	for (TInt i=0; i<iCbmiListRequests->Count(); ++i)
		{
		read = iCbmiListRequests->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iCbmiListRequests->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}

	return KErrNotFound;
	}

TInt CBroadcastMessagingDMmTsy::GetBroadcastIdListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::GetBroadcastIdListCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);

	// Remove the read all attempt from iCbmiListRequests
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iCbmiListRequests->Count(); ++i)
		{
		read = iCbmiListRequests->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iCbmiListRequests->Delete(i);
			break;
			}
		}

	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

void CBroadcastMessagingDMmTsy::StoreBroadcastIdListL(const TTsyReqHandle aTsyReqHandle, TDes8* aList)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::StoreBroadcastIdListL called"));
	
	TInt i;
	CMobilePhoneBroadcastIdList* list=CMobilePhoneBroadcastIdList::NewL();
	CleanupStack::PushL(list);
	list->RestoreL(*aList);
	if (list->Enumerate() != DMMTSY_CBMI_STORE_COUNT)
		User::Leave(KErrCorrupt);

	for (i = 0; i < list->Enumerate() && list->GetEntryL(i).iId == DMMTSY_CBMI_ID1; i++)
		{
		}
	if (i != list->Enumerate())
		User::Leave(KErrCorrupt);

	iPhone->AddDelayedReq(aTsyReqHandle,this);

	CleanupStack::PopAndDestroy(); // list
	}

TInt CBroadcastMessagingDMmTsy::StoreBroadcastIdListCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::StoreBroadcastIdListCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyBroadcastIdListChange(const TTsyReqHandle aTsyReqHandle)
	{
	if (!iNotifyBroadcastIdListChange++)
		{
		LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyBroadcastIdListChange called"));
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CBroadcastMessagingDMmTsy::NotifyBroadcastIdListChangeCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CBroadcastMessagingDMmTsy::NotifyBroadcastIdListChangeCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;	
	}



/**************************************************************************/
//
//	CUssdMessagingDMmTsy
//
/**************************************************************************/

CUssdMessagingDMmTsy* CUssdMessagingDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	{
	CUssdMessagingDMmTsy* subsession=new(ELeave) CUssdMessagingDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CUssdMessagingDMmTsy::Init()
	{}

CUssdMessagingDMmTsy::CUssdMessagingDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac, const TDesC& aName)
	:CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
	iMessagingName=aName;
	}

void CUssdMessagingDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy created"));
	}

CUssdMessagingDMmTsy::~CUssdMessagingDMmTsy()
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy destroyed"));
	}

CTelObject* CUssdMessagingDMmTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CUssdMessagingDMmTsy::OpenNewObjectL(TDes&)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CUssdMessagingDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
//
// Non-Flow Controlled Services
//
	case EMobileUssdMessagingGetCaps:
		break;
//
// Flow Controlled Services
//
	case EMobileUssdMessagingSendMessage:
	case EMobileUssdMessagingSendMessageNoFdnCheck:
	case EMobileUssdMessagingSendRelease:
		ret=KReqModeFlowControlObeyed;
		break;

//
// Immediate Server Repost
//
	case EMobileUssdMessagingReceiveMessage:
	case EMobileUssdMessagingNotifyNetworkRelease:
		ret=KReqModeRePostImmediately;
		break;
//
// Cancel Requests
//	
	case EMobileUssdMessagingReceiveMessageCancel:
	case EMobileUssdMessagingSendMessageCancel:
	case EMobileUssdMessagingSendMessageNoFdnCheckCancel:
	case EMobileUssdMessagingSendReleaseCancel:
	case EMobileUssdMessagingNotifyNetworkReleaseCancel:
	default:
		User::Leave(KErrNotSupported);
		break;
		}

	return ret;
	}

TInt CUssdMessagingDMmTsy::RegisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileUssdMessagingReceiveMessage:
		LOGTEXT(_L8("CUssdMessagingDMmTsy: RegisterNotification - Receive USSD"));
		return KErrNone;
	case EMobileUssdMessagingNotifyNetworkRelease:
		LOGTEXT(_L8("CUssdMessagingDMmTsy: RegisterNotification - Notify Network Release"));
		return KErrNone;		
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CUssdMessagingDMmTsy: Register error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CUssdMessagingDMmTsy::DeregisterNotification(const TInt aIpc)
	{
	switch (aIpc)
		{
	case EMobileUssdMessagingReceiveMessage:
		LOGTEXT(_L8("CUssdMessagingDMmTsy: DeregisterNotification - Receive USSD"));
		return KErrNone;
	case EMobileUssdMessagingNotifyNetworkRelease:
		LOGTEXT(_L8("CUssdMessagingDMmTsy: DeregisterNotification - Notify Network Release"));
		return KErrNone;
	default:
		// Unknown or invalid IPC
		LOGTEXT(_L8("CUssdMessagingDMmTsy: Deregister error, unknown IPC"));
		return KErrNotSupported;
		}
	}

TInt CUssdMessagingDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileUssdMessagingReceiveMessage:
	case EMobileUssdMessagingNotifyNetworkRelease:
		LOGTEXT(_L8("CUssdMessagingDMmTsy: Registered with 2 slot"));
		numberOfSlots=2;
		break;
	default:
		// Unknown or invalid Phone IPC
		LOGTEXT(_L8("CUssdMessagingDMmTsy: Number of Slots error, unknown IPC"));
		User::Leave(KErrNotSupported);
		break;
		}
	return numberOfSlots;
	}


TInt CUssdMessagingDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
	{
	switch (aIpc)
		{

	case EMobileUssdMessagingGetCaps:
		return GetCaps(aTsyReqHandle, aPackage.Des1n());

	case EMobileUssdMessagingSendMessage:
		return SendMessage(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());

	case EMobileUssdMessagingSendMessageNoFdnCheck:
		return SendMessageNoFdnCheck(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());

	case EMobileUssdMessagingReceiveMessage:
		return ReceiveMessage(aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n());
	case EMobileUssdMessagingSendRelease:
		return SendRelease(aTsyReqHandle, aPackage.Des1n()); 
	case EMobileUssdMessagingNotifyNetworkRelease:
		switch(aPackage.Type())
			{
		// Switch between the alternative implementations of GetPhoneStoreInfo
		case TDataPackage::EPackage1n:
			return NotifyNetworkRelease(aTsyReqHandle, aPackage.Des1n()); 
		case TDataPackage::EPackage1n2n:
			return NotifyNetworkRelease(aTsyReqHandle, aPackage.Des1n(),aPackage.Des2n()); 
		default:
			return KErrCorrupt;
			}

//
// Cancel Requests
//	
	case EMobileUssdMessagingReceiveMessageCancel:
		return ReceiveMessageCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendMessageCancel:
		return SendMessageCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendMessageNoFdnCheckCancel:
		return SendMessageNoFdnCheckCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendReleaseCancel:
		return SendReleaseCancel(aTsyReqHandle);
	case EMobileUssdMessagingNotifyNetworkReleaseCancel:
		return NotifyNetworkReleaseCancel(aTsyReqHandle);

	default:
		return KErrNotSupported;
		}
	}

TInt CUssdMessagingDMmTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy: - CancelService called"));
	switch (aIpc)
		{
	case EMobileUssdMessagingReceiveMessage:
		return ReceiveMessageCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendMessage:
		return SendMessageCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendMessageNoFdnCheck:
		return SendMessageNoFdnCheckCancel(aTsyReqHandle);
	case EMobileUssdMessagingSendRelease:
		return SendReleaseCancel(aTsyReqHandle);
	case EMobileUssdMessagingNotifyNetworkRelease:
		return NotifyNetworkReleaseCancel(aTsyReqHandle);
	default:
		return KErrGeneral; 
		}
	}

TInt CUssdMessagingDMmTsy::GetCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCaps)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::GetCaps called"));
	RMobileUssdMessaging::TMobileUssdCapsV1Pckg* ussdCapsPckg = (RMobileUssdMessaging::TMobileUssdCapsV1Pckg*)aCaps;
	RMobileUssdMessaging::TMobileUssdCapsV1& ussdCaps = (*ussdCapsPckg)();

	ussdCaps.iUssdTypes=DMMTSY_USSD_MESSAGING_TYPE_CAPS;

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::ReceiveMessage(const TTsyReqHandle aTsyReqHandle, TDes8* aMsgData, TDes8* aMsgAttributes)
	{
	if (!iReceiveMessage++)
		{
		LOGTEXT(_L8("CUssdMessagingDMmTsy::ReceiveMessage called"));
		RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileUssdMessaging::TMobileUssdAttributesV1Pckg*,aMsgAttributes);
		RMobileUssdMessaging::TMobileUssdAttributesV1& atts = (*attsPckg)();

		*aMsgData = DMMTSY_USSD_MESSAGE_PDU;

		atts.iFlags = DMMTSY_USSD_ATTRIBUTE_FLAGS;
		atts.iFormat = DMMTSY_USSD_ATTRIBUTE_FORMAT;
		atts.iType = DMMTSY_USSD_ATTRIBUTE_TYPE;
		atts.iDcs = DMMTSY_USSD_ATTRIBUTE_DCS;

		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::ReceiveMessageCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::ReceiveMessageCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::SendMessage(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDesC8* aMsgAttributes)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendMessage called"));
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileUssdMessaging::TMobileUssdAttributesV1Pckg*,aMsgAttributes);
	RMobileUssdMessaging::TMobileUssdAttributesV1& atts = (*attsPckg)();
	if ((aMsgData->Compare(DMMTSY_USSD_MESSAGE_PDU) != 0) ||
		atts.iFlags != DMMTSY_USSD_ATTRIBUTE_FLAGS || 
		atts.iFormat != DMMTSY_USSD_ATTRIBUTE_FORMAT ||
		atts.iType != DMMTSY_USSD_ATTRIBUTE_TYPE ||
		atts.iDcs != DMMTSY_USSD_ATTRIBUTE_DCS)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::SendMessageCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendMessageCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::SendMessageNoFdnCheck(const TTsyReqHandle aTsyReqHandle, TDesC8* aMsgData, TDesC8* aMsgAttributes)
	/**
	* This method sends an outgoing SMS to the network. The number used for sending the SMS   
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*
	* @param aMsgData Supplies the TPDU to send
	* @param aMsgAttributes Supplies the attributes of the outgoing message
	*/
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendMessageNoFdnCheck called"));
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* attsPckg = REINTERPRET_CAST(RMobileUssdMessaging::TMobileUssdAttributesV1Pckg*,aMsgAttributes);
	RMobileUssdMessaging::TMobileUssdAttributesV1& atts = (*attsPckg)();
	if ((aMsgData->Compare(DMMTSY_USSD_MESSAGE_PDU) != 0) ||
		atts.iFlags != DMMTSY_USSD_ATTRIBUTE_FLAGS || 
		atts.iFormat != DMMTSY_USSD_ATTRIBUTE_FORMAT ||
		atts.iType != DMMTSY_USSD_ATTRIBUTE_TYPE ||
		atts.iDcs != DMMTSY_USSD_ATTRIBUTE_DCS)
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	else
		iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::SendMessageNoFdnCheckCancel(const TTsyReqHandle aTsyReqHandle)
	/**
	* This method cancels the sending of an outgoing SMS to the network. The number used for sending the SMS   
	* is not checked against those in the Fixed Dialling Number list even if the FDN service is enabled
	*
	*/
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendMessageNoFdnCheckCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CUssdMessagingDMmTsy::SendRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendRelease called"));
	if (!iSendRelease++)
		{
		RMobilePhone::TMobilePhoneSendSSRequestV3Pckg* ssRequestPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg*,aReturnResult);
		RMobilePhone::TMobilePhoneSendSSRequestV3& ssRequest = (*ssRequestPckg)();
			
		if(ssRequest.ExtensionId()!=KETelExtMultimodeV3)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{	
			ssRequest.iOpCode = DMMTSY_PHONE_NOTIFY_OPCODE;
			ssRequest.iAdditionalInfo = DMMTSY_PHONE_NOTIFY_ADDINFO;			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	return KErrNone;
	}
	
TInt CUssdMessagingDMmTsy::SendReleaseCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::SendReleaseCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CUssdMessagingDMmTsy::NotifyNetworkRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::NotifyNetworkRelease (deprecated version) called"));
	if (!iNetworkRelease++)
		{
		RMobilePhone::TMobilePhoneSendSSRequestV3Pckg* ssRequestPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg*,aReturnResult);
		RMobilePhone::TMobilePhoneSendSSRequestV3& ssRequest = (*ssRequestPckg)();
			
		if(ssRequest.ExtensionId()!=KETelExtMultimodeV3)
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
		else
			{	
			ssRequest.iOpCode = DMMTSY_PHONE_NOTIFY_OPCODE;
			ssRequest.iAdditionalInfo = DMMTSY_PHONE_NOTIFY_ADDINFO;			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	return KErrNone;
	}
	
TInt CUssdMessagingDMmTsy::NotifyNetworkReleaseCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::NotifyNetworkReleaseCancel called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CUssdMessagingDMmTsy::NotifyNetworkRelease(const TTsyReqHandle aTsyReqHandle, TDes8* aReturnResult, TDes8* aMsgAttributes)
	{
	LOGTEXT(_L8("CUssdMessagingDMmTsy::NotifyNetworkRelease called"));
	if (iNetworkRelease >= 2)
	{
		iNetworkRelease = 0;
	}
	
	if (!iNetworkRelease++)
		{
		RMobilePhone::TMobilePhoneSendSSRequestV3Pckg* ssRequestPckg = REINTERPRET_CAST(RMobilePhone::TMobilePhoneSendSSRequestV3Pckg*,aReturnResult);
		RMobilePhone::TMobilePhoneSendSSRequestV3& ssRequest = (*ssRequestPckg)();
		
		RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* ssMsgAttributesPckg = REINTERPRET_CAST(RMobileUssdMessaging::TMobileUssdAttributesV1Pckg*,aMsgAttributes);
		RMobileUssdMessaging::TMobileUssdAttributesV1& ssMsgAttributesReq = (*ssMsgAttributesPckg)();
									
		if ((ssRequest.ExtensionId()!=KETelExtMultimodeV3) || (ssMsgAttributesReq.ExtensionId()!=KETelExtMultimodeV1))
			{
			ReqCompleted(aTsyReqHandle,KErrCorrupt);
			}
		else
			{	
			ssRequest.iOpCode = DMMTSY_PHONE_NOTIFY_OPCODE;
			ssRequest.iAdditionalInfo = DMMTSY_PHONE_NOTIFY_ADDINFO;
												
			ssMsgAttributesReq.iFlags  = DMMTSY_USSD_ATTRIBUTE_FLAGS;
			ssMsgAttributesReq.iFormat = DMMTSY_USSD_ATTRIBUTE_FORMAT;
			ssMsgAttributesReq.iType   = DMMTSY_USSD_ATTRIBUTE_TYPE;
			ssMsgAttributesReq.iDcs    = DMMTSY_USSD_ATTRIBUTE_DCS;
											
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}
	return KErrNone;
	}
