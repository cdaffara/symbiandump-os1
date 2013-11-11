// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CLtsySmsHandler/

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsysmshandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"
#include "globalphonemanager.h"
#include "atmessagestoreinfo.h"
#include "atsmssend.h"
#include "atsmsreceive.h"
#include "atsmsack.h"
#include "mslogger.h"
#include "asynchelperrequest.h"
CLtsySmsHandler::CLtsySmsHandler(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	:iCtsyDispatcherCallback(aCtsyDispatcherCallback),iGloblePhone(aGloblePhone)
	{
	
	}

	
CLtsySmsHandler::~CLtsySmsHandler()
	{
	if ( iAtSmsReceive )
		{
	    delete iAtSmsReceive;
		}
	}


CLtsySmsHandler* CLtsySmsHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsySmsHandler* self = new (ELeave) CLtsySmsHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySmsHandler* CLtsySmsHandler::NewL(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsySmsHandler* self=
			CLtsySmsHandler::NewLC(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySmsHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	iAsyncHelperRequest = CAsyncHelperRequest::NewL((*iGloblePhone), iCtsyDispatcherCallback);
	iAtSmsReceive = CAtSmsReceive::NewL(*iGloblePhone,iCtsyDispatcherCallback);
	iAtSmsReceive->StartRequest();

	} // CLtsySmsHandler::ConstructL

TBool CLtsySmsHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY 
		case KLtsyDispatchSmsGetSmsStoreInfoApiId:
		    return ETrue;
		case KLtsyDispatchSmsNackSmsStoredApiId:
			return ETrue;
		case KLtsyDispatchSmsAckSmsStoredApiId:
			return ETrue;
		case KLtsyDispatchSmsSendSmsMessageApiId:
			return ETrue;
		case KLtsyDispatchSmsSetMoSmsBearerApiId:
			return ETrue;
		default:
			return EFalse;
		}
	}

void CLtsySmsHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchSmsNotifyReceiveSmsMessageIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	
	}


TInt CLtsySmsHandler::HandleSendSatSmsReqL( const TDesC8& /*aSmsTpdu*/,
		   RMobileSmsMessaging::TMobileSmsDataFormat /*aDataFormat*/,
		   const RMobilePhone::TMobileAddress& /*aGsmServiceCentre*/,
		   TBool /*aMoreMessages*/
		  )
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp
 *
 * @param aDestination The mobile telephone number.
 * @param aSmsTpdu The SMS TPDU.
 * @param aDataFormat The SMS data format.
 * @param aGsmServiceCentre The service centre number.
 * @param aMore  This indicates whether the client is going to send another SMS immediately 
	                 after this one.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSatSmsReqL


TInt CLtsySmsHandler::HandleGetSmsStoreInfoReqL(/* TODO: Add correct parameteres here */)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp
 *
 * @param TODO: Add parameter in-source documentation here
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtMessageStoreInfo* atMessageStoreInfo = CAtMessageStoreInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atMessageStoreInfo);
		atMessageStoreInfo->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleGetSmsStoreInfoReqL


TInt CLtsySmsHandler::HandleGetSmspListReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsGetSmspListComp
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleGetSmspListReqL


TInt CLtsySmsHandler::HandleNackSmsStoredReqL(const TDesC8& aMsgPtr, TInt /*aRpCause*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp
 *
 * @param aMsgPtr Reference to TPDU data.
 * @param aRpCause RP cause.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsNack* atSmsNack = CAtSmsNack::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		atSmsNack->SetNackTpdu(aMsgPtr);
		iGloblePhone->SetActiveRequest(atSmsNack);
		atSmsNack->StartRequest();
		}

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleNackSmsStoredReqL

TInt CLtsySmsHandler::HandleNackSmsStoredReqL( TInt /*aRpCause*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp
 *
 * @param aMsgPtr Reference to TPDU data.
 * @param aRpCause RP cause.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
    {
    TSYLOGENTRYEXIT;

    TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
    if( KErrNone == ret )
        {
        CAtSmsNack* atSmsNack = CAtSmsNack::NewL(*iGloblePhone,iCtsyDispatcherCallback);
        //Setting Port active
        iGloblePhone->iEventSignalActive = ETrue;
        //atSmsNack->SetNackTpdu(aMsgPtr);
        iGloblePhone->SetActiveRequest(atSmsNack);
        atSmsNack->StartRequest();
        }

    return TSYLOGSETEXITERR(ret);
    } // CLtsySmsHandler::HandleNackSmsStoredReqL

TInt CLtsySmsHandler::HandleAckSmsStoredReqL(const TDesC8& /*aMsgPtr*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp
 *
 * @param aMsgPtr Contains the TPDU defined for a SMS-DELIVER-REPORT.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsAck* atSmsAck = CAtSmsAck::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsAck);
		atSmsAck->StartRequest();
		}
	
	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleAckSmsStoredReqL

TInt CLtsySmsHandler::HandleAckSmsStoredReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
    {
    TSYLOGENTRYEXIT;

    TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
    if( KErrNone == ret )
        {
        CAtSmsAck* atSmsAck = CAtSmsAck::NewL(*iGloblePhone,iCtsyDispatcherCallback);
        //Setting Port active
        iGloblePhone->iEventSignalActive = ETrue;
        iGloblePhone->SetActiveRequest(atSmsAck);
        atSmsAck->StartRequest();
        }
    
    return TSYLOGSETEXITERR(ret);
    }

TInt CLtsySmsHandler::HandleResumeSmsReceptionReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp
 *
 * @param TODO: Add parameter in-source documentation here
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleResumeSmsReceptionReqL


TInt CLtsySmsHandler::HandleSendSmsMessageReqL( const RMobilePhone::TMobileAddress& aDestination,
	     									    const TDesC8& aSmsTpdu,
	     									    RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
	     										const RMobilePhone::TMobileAddress& aGsmServiceCentre,
	     										TBool aMoreMessages
	   										   ) 
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp
 *
 * @param aDestination The mobile telephone number.
 * @param aSmsTpdu The SMS TPDU.
 * @param aDataFormat The SMS data format.
 * @param aGsmServiceCentre The service centre number.
 * @param aMore This indicates whether the client is going to send another SMS immediately 
	            after this one. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	if(aSmsTpdu.Length() > KGsmTpduSize)
		{
		return TSYLOGSETEXITERR(KErrArgument);
		}
	TSmsSendParam msgParam;
	msgParam.iDataFormat = aDataFormat;
	msgParam.iDestination = aDestination;
	msgParam.iSmsTpdu = aSmsTpdu;
	msgParam.iGsmServiceCentre = aGsmServiceCentre;
	msgParam.iMoreMessages = aMoreMessages;
	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtSmsSend* atSmsSend = CAtSmsSend::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atSmsSend);
		atSmsSend->SetMessageParam(msgParam);
		atSmsSend->StartRequest();
		}
	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSmsMessageReqL

TInt CLtsySmsHandler::HandleSendSmsMessageNoFdnCheckReqL( const RMobilePhone::TMobileAddress& /*aDestination*/,
	     												  const TDesC8& /*aSmsTpdu*/,
	     												  RMobileSmsMessaging::TMobileSmsDataFormat /*aDataFormat*/,
	     												  const RMobilePhone::TMobileAddress& /*aGsmServiceCentre*/,
	     												  TBool /*aMoreMessages*/
	   													) 
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp
 *
 * @param aDestination The mobile telephone number.
 * @param aSmsTpdu The SMS TPDU.
 * @param aDataFormat The SMS data format.
 * @param aGsmServiceCentre The service centre number.
 * @param aMore This indicates whether the client is going to send another SMS immediately 
	            after this one. 
	                 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSmsMessageNoFdnCheckReqL

TInt CLtsySmsHandler::HandleSetMoSmsBearerReqL(RMobileSmsMessaging::TMobileSmsBearer /*aBearer*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp
 *
 * 
 * @param aBearer The SMS bearer.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNone;
	iAsyncHelperRequest->SetRequestId(MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSetMoSmsBearerReqL


TInt CLtsySmsHandler::HandleStoreSmspListEntryReqL(const RMobileSmsMessaging::TMobileSmspEntryV1& /*aSmsParam*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsStoreSmspListEntryComp
 *
 * @param aSmsParam Defines a set of SMS parameters.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNone;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleStoreSmspListReqL

