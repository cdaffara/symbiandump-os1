// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the method definitions for all EtelMM subsessions that
// provide messaging functionality.
// 
//

/**
 @file
*/

// From core API
#include <et_clsvr.h>
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include <mmlist.h>
#include <mmretrieve.h>

#include "mm_hold.h"


/************************************************************************/
//
//  RMobileSmsMessaging
//
/************************************************************************/

EXPORT_C RMobileSmsMessaging::RMobileSmsMessaging() :
	iSmsMessagingPtrHolder(NULL)
/** 
	Standard constructor.
	This member is internal and not intended for public use.
*/
	{
	}

EXPORT_C TInt RMobileSmsMessaging::Open(RMobilePhone& aPhone)

/**
	This member function opens a SMS Messaging object. The name of the sub-session 
	opened in the TSY will equal the string defined by the KETelSmsMessaging LIT 
	definition. 
	
	@param aPhone An existing RMobilePhone session.
	@return KErrNone if successful.
 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL()); 
	if (ret)
		return ret;
	TPtrC name(KETelSmsMessaging);	
	SetSessionHandle(*session);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileSmsMessaging::Close()
/**
	This method closes a RMobileSmsMessaging subsession
 
@capability None
*/
 	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RMobileSmsMessaging::ConstructL()
/**	Constructor. */
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iSmsMessagingPtrHolder = CSmsMessagingPtrHolder::NewL(CSmsMessagingPtrHolder::EMaxNumberSmsPtrSlots,CSmsMessagingPtrHolder::EMaxNumberSmsPtrCSlots);
	}

EXPORT_C void RMobileSmsMessaging::Destruct()
/** Destructor. */
	{
	delete iSmsMessagingPtrHolder;
	iSmsMessagingPtrHolder = NULL;
	ResetSessionHandle();
	}

RMobileSmsMessaging::TMobileSmsAttributesV1::TMobileSmsAttributesV1() 
:	iFlags(0), 
	iDataFormat(EFormatUnspecified),
	iCdmaTeleservice(0),
	iCdmaServiceCategory(0)
/** Constructor. */
	{
	}

EXPORT_C RMobileSmsMessaging::TMobileSmsReceiveAttributesV1::TMobileSmsReceiveAttributesV1() 
:	iStatus(EMtMessageUnknownStatus),
	iStoreIndex(0)
/** Constructor. */
	{
	iExtensionId=KETelMobileSmsReceiveAttributesV1;
	}

EXPORT_C RMobileSmsMessaging::TMobileSmsSendAttributesV1::TMobileSmsSendAttributesV1() 
:	iMsgRef(0),
	iMore(EFalse)
/** Constructor. */
	{
	iExtensionId=KETelMobileSmsSendAttributesV1;
	}

EXPORT_C RMobileSmsMessaging::TMobileSmsCdmaSendAttributesV4::TMobileSmsCdmaSendAttributesV4() 
:	iMsgRef(0), 
	iMore(EFalse),
	iAddressAuthentication(0)
/** Constructor. */
	{
	iExtensionId=KETelMobileSmsCdmaSendAttributesV4;
	}

EXPORT_C RMobileSmsMessaging::TMobileSmsCapsV1::TMobileSmsCapsV1() 
:	iSmsMode(0), 
	iSmsControl(0)
/** Constructor. */
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileSmsMessaging::GetCaps(TDes8& aCaps) const
/**
	This member function gets a class that reflects the SMS messaging capabilities
	of the phone.
	
	@param aCaps On return, a TMobileSmsCapsV1Pckg with the SMS messaging capabilities
	of the phone.
	@return KErrNone if successful.
	@see TMobileSmsCapsV1
 
@capability None
*/
	{
	return Get(EMobileSmsMessagingGetCaps, aCaps);
	}

EXPORT_C TInt RMobileSmsMessaging::GetReceiveMode(TMobileSmsReceiveMode& aReceiveMode) const
/**
	This member function retrieves the current setting of the TSY-phone incoming 
	SMS receive mode. This setting dictates how the phone will handle incoming 
	SMS messages before passing them on to TSY.
	
	@param aReceiveMode On return, the current receive mode.
	@return KErrNone if successful.
 
@capability None
*/
	{
	TPckg<TMobileSmsReceiveMode> ptr1(aReceiveMode);
	return Get(EMobileSmsMessagingGetReceiveMode, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::SetReceiveMode(TRequestStatus& aReqStatus, TMobileSmsReceiveMode aReceiveMode) const

/**
	This member function sets the setting of the TSY-phone incoming SMS receive 
	mode. This setting dictates how the phone will handle incoming SMS messages 
	before passing them on to TSY.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode) 
	to cancel a previously placed asynchronous SetReceiveMode() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aReceiveMode The receive mode to set.
 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	iSmsMessagingPtrHolder->iReceiveMode = aReceiveMode;
	TPtrC8& ptr1=iSmsMessagingPtrHolder->SetC(CSmsMessagingPtrHolder::ESlot1SetReceiveMode,iSmsMessagingPtrHolder->iReceiveMode);

	Set(EMobileSmsMessagingSetReceiveMode, aReqStatus, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::NotifyReceiveModeChange(TRequestStatus& aReqStatus, TMobileSmsReceiveMode& aReceiveMode)
/**
	This member function is used to notify the client if there is a change to the 
	setting of the TSY-Phone receive mode.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingNotifyReceiveModeChange) 
	to cancel a previously placed asynchronous NotifyReceiveModeChange() request.
	
	@param aReqStatus On return, KErrNone if successful
	@param aReceiveMode On return, the new SMS receive mode.
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iSmsMessagingPtrHolder->Set(CSmsMessagingPtrHolder::ESlot1NotifyReceiveModeChange,aReceiveMode);

	Get(EMobileSmsMessagingNotifyReceiveModeChange, aReqStatus, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const
/**
	This member function will be used to wait for the next incoming SMS message. 
	Once this member function completes, the client should immediately repost 
	it if they want to receive the next incoming SMS message.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingReceiveMessage) 
	to cancel a previously placed asynchronous ReceiveMessage() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aMsgData On completion, the message data.
	@param aMsgAttributes On completion, a TMobileSmsReceiveAttributesV1Pckg containing 
	the message attributes.
	@see TMobileSmsReceiveAttributesV1
 
@capability NetworkServices
@capability ReadUserData
*/
	{
	Get(EMobileSmsMessagingReceiveMessage, aReqStatus, aMsgData, aMsgAttributes);
	}

EXPORT_C void RMobileSmsMessaging::AckSmsStored(TRequestStatus& aReqStatus, const TDesC8& aMsgData, TBool aFull) const
/**
	When the client receives an unstored SMS, it will use this member function 
	to return a positive acknowledgment if it manages to successfully decode and 
	store that SMS. If the client wishes to send a SMS-DELIVER-REPORT in response 
	to the received SMS-DELIVER , than the aMsgData parameter will be used to 
	hold the TPDU defined for a SMS-DELIVER-REPORT (for RP-ACK) in GSM 03.40.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingAckSmsStored) 
	to cancel a previously placed asynchronous AckSmsStored() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aMsgData Holds the TPDU defined for a SMS-DELIVER-REPORT (for RP-ACK) 
	in GSM 03.40.
	@param aFull Indicates whether, after storing the acknowledged message, the 
	client-side message store is now full or not. If client sets it to ETrue then 
	it is informing the TSY that the store is full (or almost full) and that it 
	does not want to receive any more messages until it calls ResumeSmsReception().
 
@capability NetworkServices
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iSmsMessagingPtrHolder->iAckSmsStoredFull = aFull;
	TPtrC8& ptr1=iSmsMessagingPtrHolder->SetC(CSmsMessagingPtrHolder::ESlot1AckSmsStored,iSmsMessagingPtrHolder->iAckSmsStoredFull);

	Set(EMobileSmsMessagingAckSmsStored, aReqStatus, aMsgData ,ptr1);
	}

EXPORT_C void RMobileSmsMessaging::NackSmsStored(TRequestStatus& aReqStatus, const TDesC8& aMsg, TInt aRpCause) const
/** When the client receives an unstored SMS, it will use this member function 
	to return a negative acknowledgment if it fails to decode and/or store that 
	SMS.
	
	This TPDU will contain the TP error cause that prevents the client from acknowledging 
	the SMS. The client should also provide the RP error cause by placing it in 
	the aRpCause parameter. 
	
	In GSM mode, once a client has rejected a received SMS due to storage full, 
	the network will keep this message for later and suspend the forwarding of 
	any further received SMS to this MS.
	
	In CDMA mode, the network will save the message but it will repeat sending 
	of the message to the MS until either the MS eventually accepts it or the 
	retry limit is reached.
	
	If retry limit is reached the network will tell the originator of the message 
	that the transmission failed. In either case, when client-side memory is freed-up, 
	the client must call ResumeSmsReception() in order to tell the phone and possibly 
	the network that SMS can be received again.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingNackSmsStored) 
	to cancel a previously placed asynchronous NackSmsStored() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aMsg Holds the TPDU defined for a SMS-DELIVER-REPORT (for RP-ERROR) 
	in GSM 03.40.
	@param aRpCause Contain one of the MS related extended error codes.
 
@capability NetworkServices
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	iSmsMessagingPtrHolder->iNackSmsStoredCause = aRpCause;
	TPtrC8& ptr1=iSmsMessagingPtrHolder->SetC(CSmsMessagingPtrHolder::ESlot1NackSmsStored,iSmsMessagingPtrHolder->iNackSmsStoredCause);

	Set(EMobileSmsMessagingNackSmsStored, aReqStatus, aMsg, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::ResumeSmsReception(TRequestStatus& aReqStatus) const
/**
	This member functions resumes the reception of SMS messages.
	
	SMS reception can become suspended because SMS storage is full. Therefore 
	if client-side storage is full, the client must stop receiving unstored messages. 
	However, if the user deletes some client-side stored SMS messages, the client 
	must use this member function to tell the TSY to inform a GSM network that 
	messages can be received again or the phone that CDMA messages can be accepted 
	again.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception) 
	to cancel a previously placed asynchronous ResumeSmsReception() request.
	
	@param aReqStatus On return, KErrNone if successful.
 
@capability NetworkServices
@capability NetworkControl
*/
	{
	Blank(EMobileSmsMessagingResumeSmsReception, aReqStatus);
	}

EXPORT_C void RMobileSmsMessaging::SendMessage(TRequestStatus& aReqStatus,  const TDesC8& aMsgData, TDes8& aMsgAttributes) const
/**
	This member function sends an SMS message to the network.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingSendMessage) 
	to cancel a previously placed asynchronous SendMessage() request.
	
	@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
	phone does not support outgoing SMS, KErrMMEtelFormatNotSupported if the phone 
	does not support the client's chosen message data format, any of the KErrGsmSms… 
	extended errors if the phone is in GSM mode and this is the error returned 
	by the GSM network/service centre, Any of the KErrCdmaSms… extended errors 
	if the phone is in CDMA mode and this is the error returned by the CDMA network/service 
	centre. Please note that the last three examples are "extended" errors and 
	will only be returned to those clients that request them. Clients that only 
	understand core Symbian OS and ETel errors will receive KErrGeneral for any 
	error cases that the TSY can not interpret.
	@param aMsgData The message data
	@param aMsgAttributes A TMobileSmsSendAttributesV1Pckg with the message attributes. 
	The fields required will probably be message data format, destination and 
	possibly a new service centre address. On return, for either successful or 
	failed transmission, the message reference number assigned to this sent message 
	will be placed by the TSY in the iMsgRef field. Also, if the SMS was sent 
	during GSM or WCDMA mode, then the SMS-SUBMIT-REPORT will be placed by the 
	TSY in the iSubmitReport field.
	@see TMobileSmsSendAttributesV1
 
@capability NetworkServices
@capability WriteUserData
*/
	{
	SetAndGet(EMobileSmsMessagingSendMessage, aReqStatus,aMsgData,aMsgAttributes);
	}

EXPORT_C void RMobileSmsMessaging::SendMessageNoFdnCheck(TRequestStatus& aReqStatus,  const TDesC8& aMsgData, TDes8& aMsgAttributes) const
/**
	Sends an outgoing SMS to the network. 
	
	The number used for sending the SMS is not checked against those in the 
	Fixed Dialling Number list even if the FDN service is enabled.
	
	@param aMsgData Supplies the TPDU to send
	@param aMsgAttributes Supplies the attributes of the outgoing message
 
@capability NetworkServices
@capability NetworkControl
*/
	{
	SetAndGet(EMobileSmsMessagingSendMessageNoFdnCheck, aReqStatus,aMsgData,aMsgAttributes);
	}

EXPORT_C void RMobileSmsMessaging::SetMoSmsBearer(TRequestStatus& aReqStatus, TMobileSmsBearer aBearer) const
/**
	This member function sets the bearer type (GSM or GPRS) for sending SMS messages.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingSetMoSmsBearer) 
	to cancel a previously placed asynchronous SetMoSmsBearer() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aBearer The bearer type to set.
 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	iSmsMessagingPtrHolder->iSmsBearer = aBearer;
	TPtrC8& ptr1=iSmsMessagingPtrHolder->SetC(CSmsMessagingPtrHolder::ESlot1SetMoSmsBearer,iSmsMessagingPtrHolder->iSmsBearer);

	Set(EMobileSmsMessagingSetMoSmsBearer, aReqStatus, ptr1);
	}

EXPORT_C TInt RMobileSmsMessaging::GetMoSmsBearer(TMobileSmsBearer& aBearer) const
/**
	This member function is used to retrieve the current setting for the bearer 
	type (GSM or GPRS) used for sending SMS messages. 
	
	@param aBearer On return, the current bearer type.
	@return KErrNone if successful.
 
@capability None
*/
	{
	TPckg<TMobileSmsBearer> ptr1(aBearer);
	return Get(EMobileSmsMessagingGetMoSmsBearer, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::NotifyMoSmsBearerChange(TRequestStatus& aReqStatus, TMobileSmsBearer& aBearer)
/**
	This member function is used t notify the client if there is a change to the 
	setting for the bearer type used for sending SMS messages. 
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingNotifyMoSmsBearerChange) 
	to cancel a previously placed asynchronous NotifyMoSmsBearerChange() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aBearer On completion, the new bearer.
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iSmsMessagingPtrHolder->Set(CSmsMessagingPtrHolder::ESlot1NotifyMoSmsBearerChange,aBearer);

	Get(EMobileSmsMessagingNotifyMoSmsBearerChange, aReqStatus, ptr1);
	}

//
// SMS related storage
//

EXPORT_C TInt RMobileSmsMessaging::EnumerateMessageStores(TInt& aCount) const

/**
	This request gets the number of phone-side SMS message stores supported by 
	the phone in the aCount parameter.
	
	@param aCount On return, the number of phone-side SMS message stores.
	@return KErrNone if successful.
 
@capability None
*/
	{
	TPckg<TInt> ptr1(aCount);
	return Get(EMobileSmsMessagingEnumerateMessageStores, ptr1);
	}

EXPORT_C void RMobileSmsMessaging::GetMessageStoreInfo(TRequestStatus& aReqStatus, TInt aIndex, TDes8& aInfo) const
/**
	This request gets information about the SMS message store.
	
	Use	RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo) 
	to cancel a previously placed asynchronous GetMessageStoreInfo() request.
	
	@param aReqStatus On return, KErrNone if successful
	@param aIndex The index of the store, valid values are between 0 and (n-1), 
	where n is given by the EnumerateMessageStores() request.
	@param aInfo On comletion, a RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg 
	holding the SMS message store info.
	@see RMobilePhoneStore::TMobilePhoneStoreInfoV1
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	
	iSmsMessagingPtrHolder->iGetMessageStoreInfoIndex = aIndex;
	TPtrC8& ptr1=iSmsMessagingPtrHolder->SetC(CSmsMessagingPtrHolder::ESlot1GetMessageStoreInfo,iSmsMessagingPtrHolder->iGetMessageStoreInfoIndex);

	SetAndGet(EMobileSmsMessagingGetMessageStoreInfo, aReqStatus, ptr1, aInfo);
	}

//
// SMS Parameter Storage
//

EXPORT_C RMobileSmsMessaging::TMobileSmspEntryV1::TMobileSmspEntryV1() 
: 	iValidParams(0), 
	iProtocolId(0), 
	iDcs(0), 
	iValidityPeriod(0),
	iReservedFiller(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

void RMobileSmsMessaging::TMobileSmspEntryV1::InternalizeL(RReadStream& aStream)
/**
	This method internalizes the SMS parameter entry from a stream 
	
	@param aStream The read stream containing the SMS parameter entry
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iIndex = aStream.ReadInt32L();
	aStream >> iText;
	iValidParams = aStream.ReadUint32L();
	aStream >> iDestination;
	aStream >> iServiceCentre;
	iProtocolId = aStream.ReadUint8L();
	iDcs = aStream.ReadUint8L();
	iValidityPeriod = aStream.ReadUint8L();
	}

void RMobileSmsMessaging::TMobileSmspEntryV1::ExternalizeL(RWriteStream& aStream) const
/**
	This method externalizes the SMS parameter entry into a stream 
	@param aStream The write stream that will contain the SMS parameter entry
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteInt32L(iIndex);
	aStream << iText;
	aStream.WriteUint32L(iValidParams);
	aStream << iDestination;
	aStream << iServiceCentre;
	aStream.WriteUint8L(iProtocolId);
	aStream.WriteUint8L(iDcs);
	aStream.WriteUint8L(iValidityPeriod);
	}


EXPORT_C void RMobileSmsMessaging::StoreSmspListL(TRequestStatus& aReqStatus, CMobilePhoneSmspList* aSmspList) const
/**
	This member function may be used to store a new version of the entire list 
	of SMSP entries. It will overwrite the current contents of the storage and 
	may cause other clients to be notified of the change in SMSP list.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingStoreSmspList) 
	to cancel a previously placed asynchronous StoreSmspListL() request.
	
	@leave k Will leave if the CBufFlat to hold the streamed contents can not be allocated
	@param aReqStatus On return, KErrNone if successful.
	@param aSmspList The list of SMSP entries to store, should not contain more 
	entries than can be stored phone-side.
 
@capability WriteDeviceData
*/
	{
	__ASSERT_ALWAYS(aSmspList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iSmsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	delete iSmsMessagingPtrHolder->iSmspBuf;
	iSmsMessagingPtrHolder->iSmspBuf = NULL;

	iSmsMessagingPtrHolder->iSmspBuf=aSmspList->StoreLC();
	CleanupStack::Pop();

	(iSmsMessagingPtrHolder->iSmspPtr).Set((iSmsMessagingPtrHolder->iSmspBuf)->Ptr(0));

	Set(EMobileSmsMessagingStoreSmspList,aReqStatus,iSmsMessagingPtrHolder->iSmspPtr);

	}

EXPORT_C void RMobileSmsMessaging::NotifySmspListChange(TRequestStatus& aReqStatus) const
/**
	This member function allows a client to be notified that changes have been 
	made to the SMSP list. When it completes, it does not return the new version 
	of the list so the client will have to retrieve the list again if it is interested 
	in the new list.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileSmsMessagingNotifySmspListChange) 
	to cancel a previously placed asynchronous NotifySmspListChange() request.
	
	@param aReqStatus On return, KErrNone if successful.
 
@capability None
*/
	{
	Blank(EMobileSmsMessagingNotifySmspListChange,aReqStatus);
	}


/************************************************************************/
//
//  RMobileBroadcastMessaging
//
/************************************************************************/

EXPORT_C RMobileBroadcastMessaging::RMobileBroadcastMessaging() :
	iCbsMessagingPtrHolder(NULL)
/** Standard constructor. */
	{
	}

EXPORT_C TInt RMobileBroadcastMessaging::Open(RMobilePhone& aPhone)
/**
	This function member opens a Broadcast Messaging object. The name of the sub-session 
	opened in the TSY will equal the string defined by the KETelBroadcastMessaging 
	LIT definition. 
	
	@param aPhone The existing RMobilePhone session.
	@return KErrNone if successful, a system wide error code if not.
 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL()); 
	if (ret)
		return ret;
	TPtrC name(KETelBroadcastMessaging);	
	SetSessionHandle(*session);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);	
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileBroadcastMessaging::Close()
/** This function member closes a RMobileBroadcastMessaging object. 
@capability None
*/
 	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RMobileBroadcastMessaging::ConstructL()
	{
	__ASSERT_ALWAYS(iCbsMessagingPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iCbsMessagingPtrHolder = CCbsMessagingPtrHolder::NewL(CCbsMessagingPtrHolder::EMaxNumberBroadcastPtrSlots,CCbsMessagingPtrHolder::EMaxNumberBroadcastPtrCSlots);
	}

EXPORT_C void RMobileBroadcastMessaging::Destruct()
/** Destructor. Deletes any dynamically allocated memory. */
	{
	delete iCbsMessagingPtrHolder;
	iCbsMessagingPtrHolder = NULL;
	ResetSessionHandle();
	}

EXPORT_C RMobileBroadcastMessaging::TMobileBroadcastCapsV1::TMobileBroadcastCapsV1() : 
	iModeCaps(0), 
	iFilterCaps(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileBroadcastMessaging::GetCaps(TDes8& aCaps) const
/**
	This function member gets a class that reflects the broadcast messaging capabilities 
	of the phone.
	
	@param aCaps On completion, a TMobileBroadcastCapsV1Pckg with the broadcast 
	messaging capabilities.
	@return KErrNone if successful, a system wide error code if not.
	@see TMobileBroadcastCapsV1
 
@capability None
*/
	{
	return Get(EMobileBroadcastMessagingGetCaps, aCaps);
	}

EXPORT_C RMobileBroadcastMessaging::TMobileBroadcastAttributesV1::TMobileBroadcastAttributesV1() :
	iFlags(0), 
	iFormat(EFormatUnspecified),
	iServiceCategory(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

/**
	This is the initialization of the TMobileBroadcastAttributesV2 structured type
	
*/
EXPORT_C RMobileBroadcastMessaging::TMobileBroadcastAttributesV2::TMobileBroadcastAttributesV2() :	
	TMobileBroadcastAttributesV1(),
	iNumberOfPages(0),
	iMessageType(0),
	iMessageId(0),
	iSerialNum(0),
	iDCS(0)
	/** Default constructor. All internal data is set to 0 and the extension ID to 
	KETelExtMultimodeV2. */
	{
	iExtensionId=KETelExtMultimodeV2;
	}


/**
	This function member enables the client to receive the next 
	incoming broadcast message from the network.
	
	The request will be completed when a new message arrives. The
	client will pass a descriptor of size KBroadcastPageSize bytes, 
	provided by the typedef TBroadcastPageData, to hold the returned
	broadcast message data into the aMsgData parameter.
	
	This buffer size should be used for all types of messages:
	GSM, CDMA and WCDMA.
	
	The function explicity checks that the aMsgData parameter is of
	size KBroadcastPageSize.  If the buffer size does not equal 
	KBroadcastPageSize, the client request will be completed with
	KErrArgument.
	
	A TSY should handle ReceiveMessage() requests by making them repost
	immediately (to ensure none are missed) and possibly multiple 
	completion enabled. When a new message arrives the TSY will copy the
	broadcast message contents into its stored pointer to the broadcast 
	message data, fill in the broadcast message attributes and then 
	complete the request.
	
	@param aReqStatus On return, KErrNone if successful, KErrArgument if the aMsgData buffer size does not equal KBroadcastPageSize, otherwise a system wide error code.
	@param aMsgData On completion, the message data.
	@param aMsgAttributes On completion, the broadcast message attributes, in an instance of the TMobileBroadcastAttributesV1Pckg class.
	@see TMobileBroadcastAttributesV1
 
@capability None
*/
EXPORT_C void RMobileBroadcastMessaging::ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const
	{
	if (aMsgData.MaxLength() != KBroadcastPageSize)
		{
		TRequestStatus* status=&aReqStatus;
		User::RequestComplete(status,KErrArgument);
		return;
		}
	Get(EMobileBroadcastMessagingReceiveMessage, aReqStatus, aMsgData, aMsgAttributes);
	}

EXPORT_C TInt RMobileBroadcastMessaging::GetFilterSetting(TMobilePhoneBroadcastFilter& aSetting) const
/**
	This function member gets the current setting for the receipt of broadcast 
	messages.
	
	This setting will indicate whether all or no messages are being accepted or 
	whether only those messages specified by the filters are going to be accepted 
	or rejected.
	
	If the returned setting, aSetting, equals EBroadcastAcceptFilter then the 
	phone will accept messages whose identifier is included within the identifier 
	list AND whose language is included within the language filter. If the returned 
	setting, aSetting, equals EBroadcastRejectFilter then the phone will reject 
	messages whose identifier is included within the identifier list OR whose 
	language is included within the language filter.
	
	N.B. If the phone supports both GSM/WCDMA and CDMA mode then it will probably 
	need two identifier lists - one to contain a list of CBMI (for GSM/WCDMA mode) 
	and the other to contain a list of Service Categories (for CDMA mode) This 
	function member is synchronous because it is expected that this will be a 
	TSY specific setting whose current value the TSY has previously determined.
	
	@param aSetting On completion, the current filter settings.
	@return KErrNone if successful, or KErrNotSupported if the phone does not support 
	a filter setting.
 
@capability ReadUserData
*/
	{
	TPckg<TMobilePhoneBroadcastFilter> ptr1(aSetting);
	return Get(EMobileBroadcastMessagingGetFilterSetting, ptr1);
	}

EXPORT_C void RMobileBroadcastMessaging::SetFilterSetting(TRequestStatus& aReqStatus, TMobilePhoneBroadcastFilter aSetting) const
/**
	This function member sets a new setting for the receipt of broadcast messages.
	
	This setting will specify whether all or no messages are being accepted or 
	whether only those messages specified by the filters are going to be accepted 
	or rejected.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingSetFilterSetting) 
	to cancel a previously placed asynchronous SetFilterSetting() request.
	
	@param aReqStatus On return, the KErrNone if successful, KErrNotSupported 
	if the phone does not support a filter setting. 
	@param aSetting On completion, the new filter setting of the phone.
 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(iCbsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	iCbsMessagingPtrHolder->iSetFilterSetting=aSetting;

	TPtrC8& ptr1=iCbsMessagingPtrHolder->SetC(CCbsMessagingPtrHolder::ESlot1SetFilterSetting, iCbsMessagingPtrHolder->iSetFilterSetting);
	
	Set(EMobileBroadcastMessagingSetFilterSetting, aReqStatus, ptr1);
	}

EXPORT_C void RMobileBroadcastMessaging::NotifyFilterSettingChange(TRequestStatus& aReqStatus, TMobilePhoneBroadcastFilter& aSetting) const
/**
	This function member allows a client to be notified if there is a change in 
	the setting for the receipt of broadcast messages.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingNotifyFilterSettingChange) 
	to cancel a previously placed asynchronous NotifyFilterSetting() request.
	
	@param aReqStatus On return, KErrNone if successful, KErrNotSupported if the 
	phone does not support a filter setting.
	@param aSetting On completion, the changed filter setting.
 
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(iCbsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	TPtr8& ptr1=iCbsMessagingPtrHolder->Set(CCbsMessagingPtrHolder::ESlot1NotifyFilterSettingChange,aSetting);

	Get(EMobileBroadcastMessagingNotifyFilterSettingChange, aReqStatus, ptr1);
	}

EXPORT_C void RMobileBroadcastMessaging::GetLanguageFilter(TRequestStatus& aReqStatus, TDes16& aLangFilter) const
/**
	This function member allows a client to retrieve the current contents of the 
	broadcast message language filter.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingGetLanguageFilter) 
	to cancel a previously placed asynchronous GetLanguageFilter() request.
	
	@param aReqStatus On return KErrNone if successful, KErrNotFound if there 
	is no language filter in place, and KErrNotSupported if the phone does not 
	support a language filter.
	@param aLangFilter On completion, contain as many languages (each coded on 
	16-bits) as can fit in the descriptor supplied by the client. This filter 
	defines which are the desired (or undesired) language(s) of incoming broadcast 
	messages. A language is defined either within one byte by its Cell Broadcast 
	Data Coding Scheme (as defined in GSM 03.38) or within two bytes by its ISO 
	639 alphanumeric pair. Even if a language is represented by an 8-bit value 
	it will be returned as a 16-bit value with the language in the least significant 
	8-bits.
 
@capability ReadUserData
*/
	{
	Get(EMobileBroadcastMessagingGetLanguageFilter,aReqStatus,aLangFilter);
	}

EXPORT_C void RMobileBroadcastMessaging::SetLanguageFilter(TRequestStatus& aReqStatus, const TDesC16& aLangFilter) const
/**
	This function member allows a client to store new contents for the broadcast 
	message language filter.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingSetLanguageFilter) 
	to cancel a previously placed asynchronous SetLanguageFilter() request.
	
	@param aReqStatus On return, KErrNone, if successful, KErrNotSupported if 
	the phone does not support a language filter.
	@param aLangFilter Each element in this descriptor will define a language. 
	This filter will define which are the desired (or undesired) language(s) of 
	incoming broadcast messages.
 
@capability WriteUserData
*/
	{
	Set(EMobileBroadcastMessagingSetLanguageFilter,aReqStatus,aLangFilter);
	}

EXPORT_C void RMobileBroadcastMessaging::NotifyLanguageFilterChange(TRequestStatus& aReqStatus, TDes16& aLangFilter) const
/**
	This function member allows a client to be notified if there is a change in 
	the contents of the language filter.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingNotifyLanguageFilterChange) 
	to cancel a previously placed asynchronous NotifyLanguageFilterChange() request.
	
	@param aReqStatus On return, KErrNone, if successful, KErrNotSupported if 
	the phone does not support a language filter.
	@param aLangFilter On completion, the new language filter.
 
@capability ReadUserData
*/
	{
	Get(EMobileBroadcastMessagingNotifyLanguageFilterChange,aReqStatus,aLangFilter);
	}

EXPORT_C RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1::TMobileBroadcastIdEntryV1() :
	iId(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

void RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1::InternalizeL(RReadStream& aStream)
/** Internalizes the CBMI entry from a stream.

	@param aStream The read stream containing the CBMI entry.
 */
	{
	TMultimodeType::InternalizeL(aStream);
	iId = aStream.ReadUint16L();
	}

void RMobileBroadcastMessaging::TMobileBroadcastIdEntryV1::ExternalizeL(RWriteStream& aStream) const
/** Externalizes the CBMI entry into a stream.
	
	@param aStream The write stream that will contain the CBMI entry.
 */
	{
	TMultimodeType::ExternalizeL(aStream);
	aStream.WriteInt16L(iId);
	}

EXPORT_C void RMobileBroadcastMessaging::StoreBroadcastIdListL(TRequestStatus& aReqStatus, CMobilePhoneBroadcastIdList* aBroadcastIdList, TMobileBroadcastIdType aIdType)
/**
	This function member may be used to store a new version of the entire BroadcastId list 
	of identifier entries.
	
	The list, should not contain more entries than can be stored phone-side. This 
	function member will store all the identifier list entries into the phone-side 
	storage. It will overwrite the current contents of the storage and may cause 
	other clients to be notified of the change in identifier list.
	
	Due to the probable time taken to store a list, there is only an asynchronous 
	version of this function member. This function member will leave if an out 
	of memory error occurs during allocation of a client-side buffer in which 
	to store the streamed list contents. 
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingStoreIdList) 
	to cancel a previously placed asynchronous StoreBroadcastIdListL() request.
	Will leave if the CBufFlat to hold the streamed contents can not be allocated
	
	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
	@param aBroadcastIdList The list of BroadcastId identifier entries to be stored.
	@param aIdType Specifies whether the list is to be stored as a GSM/WCDMA list 
	of CBMI entries or a CDMA list of Service Category entries.
 
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(aBroadcastIdList!=NULL,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(iCbsMessagingPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));

	delete iCbsMessagingPtrHolder->iBroadcastIdBuf;
	iCbsMessagingPtrHolder->iBroadcastIdBuf = NULL;

	iCbsMessagingPtrHolder->iBroadcastIdBuf=aBroadcastIdList->StoreLC();
	CleanupStack::Pop();

	(iCbsMessagingPtrHolder->iBroadcastIdPtr).Set((iCbsMessagingPtrHolder->iBroadcastIdBuf)->Ptr(0));

	iCbsMessagingPtrHolder->iIdType = aIdType;
	TPtrC8& ptr2=iCbsMessagingPtrHolder->SetC(CCbsMessagingPtrHolder::ESlot1StoreBroadcastIdListL,iCbsMessagingPtrHolder);

	Set(EMobileBroadcastMessagingStoreIdList,aReqStatus,iCbsMessagingPtrHolder->iBroadcastIdPtr, ptr2);
	}

EXPORT_C void RMobileBroadcastMessaging::NotifyBroadcastIdListChange(TRequestStatus& aReqStatus) const
/**
	This function member allows a client to be notified that changes have been 
	made to the BroadcastId filter identifier list.
	
	When it completes, it does not return the new version of the list so the client 
	will have to retrieve the list using CRetrieveMobilePhoneBroadcastIdL again if it is interested in the new list.

	Use RTelSubSessionBase::CancelAsyncRequest(EMobileBroadcastMessagingNotifyIdListChange) 
	to cancel a previously placed asynchronous NotifyBroadcastIdListChange() request.

	@param aReqStatus On return, KErrNone if successful, a system wide error code 
	if not.
 
@capability None
*/
	{
	Blank(EMobileBroadcastMessagingNotifyIdListChange,aReqStatus);
	}

/************************************************************************/
//
//  RMobileUssdMessaging
//
/************************************************************************/

EXPORT_C RMobileUssdMessaging::RMobileUssdMessaging()
: iUssdMessagingPtrHolder(NULL)
/** Constructor. */
	{
	}

EXPORT_C TInt RMobileUssdMessaging::Open(RMobilePhone& aPhone)
/**
	This member function opens a USSD messaging sub-session. The name of the sub-session 
	opened in the TSY will equal the string defined by the KETelUssdMessaging 
	LIT definition.
	
	@param aPhone An existing RMobilePhone session.
	@return KErrNone if successful.
 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL()); 
	if (ret)
		return ret;
	TPtrC name(KETelUssdMessaging);	
	SetSessionHandle(*session);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);	
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RMobileUssdMessaging::Close()
/** This member function closes a USSD Messaging object. 
@capability None
*/
 	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RMobileUssdMessaging::ConstructL()
	{
	}

EXPORT_C void RMobileUssdMessaging::Destruct()
/** Destructor. */
	{
	}

EXPORT_C RMobileUssdMessaging::TMobileUssdCapsV1::TMobileUssdCapsV1() : 
	iUssdFormat(0), 
	iUssdTypes(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C TInt RMobileUssdMessaging::GetCaps(TDes8& aCaps) const
/**
	This member function returns a class that reflects the USSD messaging capabilities 
	of the phone.
	
	@param aCaps On completion, a TMobileUssdCapsV1Pckg with the current USSD 
	messaging capabilities.
	@return KErrNone if successful.
	@see TMobileUssdCapsV1
 
@capability None
*/
	{
	return Get(EMobileUssdMessagingGetCaps, aCaps);
	}

EXPORT_C RMobileUssdMessaging::TMobileUssdAttributesV1::TMobileUssdAttributesV1() :
	iFlags(0), 
	iFormat(EFormatUnspecified), 
	iType(EUssdUnknown), 
	iDcs(0)
	{
	iExtensionId=KETelExtMultimodeV1;
	}

EXPORT_C void RMobileUssdMessaging::ReceiveMessage(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const
/**
	This member function enables the client to receive the next incoming USSD message 
	from the network. The request will be completed when a new message arrives.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileUssdMessagingReceiveMessage) 
	to cancel a previously placed asynchronous ReceiveMessage() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aMsgData A descriptor large enough to hold the returned USSD message 
	data. For GSM/ WCDMA USSD messages this will be a buffer of 160 bytes and 
	the clients are provided with a typedef called RMobileUssdMessaging::TGsmUssdMessageData.
	@param aMsgAttributes On completion, the TMobileUssdAttributesV1Pckg with the 
	message attributes.
 
	@see TMobileUssdAttributesV1

@capability ReadDeviceData
@capability NetworkControl
*/
	{
	Get(EMobileUssdMessagingReceiveMessage, aReqStatus, aMsgData, aMsgAttributes);
	}

EXPORT_C void RMobileUssdMessaging::SendMessage(TRequestStatus& aReqStatus, const TDesC8& aMsgData, const TDesC8& aMsgAttributes) const
/**
	This member function sends a USSD message to the network.

	See	3GPP spec. 04.90 for more details on expected responses from
	the network.
	
	Use RTelSubSessionBase::CancelAsyncRequest(EMobileUssdMessagingSendMessage) 
	to cancel a previously placed asynchronous SendMessage() request.
	
	@param aReqStatus On return, KErrNone if successful.
	@param aMsgData The message data.
	@param aMsgAttributes The TMobileUssdAttributesV1Pckg with the message attributes.

	@see TMobileUssdAttributesV1

@capability NetworkServices
@capability WriteDeviceData
@capability NetworkControl
*/
	{
	Set(EMobileUssdMessagingSendMessage, aReqStatus, aMsgData, aMsgAttributes);
	}

EXPORT_C void RMobileUssdMessaging::SendMessageNoFdnCheck(TRequestStatus& aReqStatus, const TDesC8& aMsgData, const TDesC8& aMsgAttributes) const
/**
	Sends an outgoing USSD to the network. 
	
	The number used for sending the SMS is not checked against those 
	in the Fixed Dialling Number list even if the FDN service is enabled.
	
	@param aMsgData Supplies the USSD message data to send
	@param aMsgAttributes Supplies the attributes of the outgoing USSD
 
@capability NetworkServices
@capability NetworkControl
@capability WriteDeviceData
*/
	{
	Set(EMobileUssdMessagingSendMessageNoFdnCheck, aReqStatus, aMsgData, aMsgAttributes);
	}

EXPORT_C void RMobileUssdMessaging::SendRelease(TRequestStatus& aReqStatus, TDes8& aReturnResult) const
/**
	Sends a USSD release message to the network.  This request can terminate an ongoing "dialogue" (send-receive) at any time.
	
	@param aReqStatus Returns the result code after the asynchronous call completes.
	@param aReturnResult A packaged instance of the RMobilePhone::TMobilePhoneSendSSRequestV3 class containing the result of the release.

@capability NetworkServices
@capability NetworkControl
*/
	{
	Get(EMobileUssdMessagingSendRelease, aReqStatus, aReturnResult);
	}
	
EXPORT_C void RMobileUssdMessaging::NotifyNetworkRelease(TRequestStatus& aReqStatus, TDes8& aReturnResult) const
/**
	Notifies the client of a MT USSD release message to the mobile device.  The notification completes when a release
	message to terminate the session has been received and successfully acknowledged.  
    NB: This fucntion has been deprecated and it is not recommended for future development work. 
	
	@param aReqStatus Returns the result code after the asynchronous call completes.
	@param aReturnResult A packaged instance of the RMobilePhone::TMobilePhoneSendSSRequestV3 class containing the result of the release.

@capability None
*/
	{
	Get(EMobileUssdMessagingNotifyNetworkRelease, aReqStatus, aReturnResult);
	}

EXPORT_C void RMobileUssdMessaging::NotifyNetworkRelease(TRequestStatus& aReqStatus, TDes8& aMsgData, TDes8& aMsgAttributes) const
/**
	Notifies the client of a MT USSD release message to the mobile device.  The notification completes when a release
	message to terminate the session has been received and successfully acknowledged.  
 
	@param aReqStatus Returns the result code after the asynchronous call completes.
	@param aMsgData A packaged instance of the RMobilePhone::TMobilePhoneSendSSRequestV3 class containing the result of the release.
    @param aMsgAttributes A packaged instance of the TMobileUssdAttributesV1Pckg class containing the message attributes.

	@see TMobileUssdAttributesV1

@capability None
*/
	{
	Get(EMobileUssdMessagingNotifyNetworkRelease, aReqStatus, aMsgData, aMsgAttributes);
	}

