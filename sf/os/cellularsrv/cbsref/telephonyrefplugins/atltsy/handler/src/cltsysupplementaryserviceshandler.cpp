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
// CLtsySupplementaryServicesHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsysupplementaryserviceshandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsySupplementaryServicesHandler::CLtsySupplementaryServicesHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsySupplementaryServicesHandler::~CLtsySupplementaryServicesHandler()
	{
	}


CLtsySupplementaryServicesHandler* CLtsySupplementaryServicesHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySupplementaryServicesHandler* self = new (ELeave) CLtsySupplementaryServicesHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySupplementaryServicesHandler* CLtsySupplementaryServicesHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySupplementaryServicesHandler* self=
			CLtsySupplementaryServicesHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySupplementaryServicesHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsySupplementaryServicesHandler::ConstructL

TBool CLtsySupplementaryServicesHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried.
 * @return indication whether interface is supported or not.
 */
	{

	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	}

void CLtsySupplementaryServicesHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1.
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
	}


TInt CLtsySupplementaryServicesHandler::HandleSendNetworkServiceRequestNoFdnCheckReqL(const TDesC& /*aRequest*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp()
 *
 * @param aRequest The supplementary services string that was entered by the user and need to be send to the network.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSendNetworkServiceRequestNoFdnCheckReqL


TInt CLtsySupplementaryServicesHandler::HandleGetCallWaitingStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallWaitingStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleGetCallWaitingStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSendNetworkServiceRequestReqL(const TDesC& /*aRequest*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestComp()
 *
 * @param aRequest The supplementary services string that was entered by the user and need to be send to the network.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSendNetworkServiceRequestReqL


TInt CLtsySupplementaryServicesHandler::HandleGetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition /*aCondition*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallBarringStatusComp()
 *
 * @param aCondition The barring conditions to interrogate.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleGetCallBarringStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition /*aCondition*/,
        RMobilePhone::TMobilePhoneServiceAction /*aAction*/,
        RMobilePhone::TMobileService /*aGroup*/,
        const TDesC& /*aPassword*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallBarringStatusComp()
 *
 * @param aCondition the call barring condition to set.
 * @param aAction the call barring action.
 * @param aGroup the call barring service group.
 * @param aPassword the password of the call barring service.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSetCallBarringStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition /*aCondition*/,
		RMobilePhone::TMobileService /*aServiceGroup*/,
		RMobilePhone::TMobilePhoneServiceAction /*aAction*/,
		const TDesC& /*aNumber*/,
		TInt /*aTimeout*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallForwardingStatusComp()
 *
 * @param aCondition the call forwarding condition to set.
 * @param aServiceGroup The basic service group to apply the CF status change to.
 * @param aAction The action to take.
 * @param aNumber The forwarding number.
 * @param aTimeout The time out when aCondition is ECallForwardingNoReply.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSetCallForwardingStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSetCallWaitingStatusReqL(RMobilePhone::TMobileService /*aServiceGroup*/,
		RMobilePhone::TMobilePhoneServiceAction /*aAction*/) 
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallWaitingStatusComp()
 *
 * @param aServiceGroup The service group to change.
 * @param aAction The action to do with that service group.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSetCallWaitingStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSetSsPasswordReqL(const TDesC& /*aOldPassword*/,
		const TDesC& /*aNewPassword*/,
		MLtsyDispatchSupplementaryServicesSetSsPassword::TServiceType /*aService*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetSsPasswordComp()
 *
 * @param aOldPassword The old password.
 * @param aNewPassword The new password.
 * @param aService The service.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSetSsPasswordReqL


TInt CLtsySupplementaryServicesHandler::HandleGetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition /*aCondition*/, RMobilePhone::TMobileService /*aService*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallForwardingStatusComp()
 *
 * @param aCondition the call forwarding condition.
 * @param aService The service group.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleGetCallForwardingStatusReqL


TInt CLtsySupplementaryServicesHandler::HandleSendUssdMessageReqL(const TDesC8& /*aData*/, TUint8 /*aDcs*/, 
		RMobileUssdMessaging::TMobileUssdDataFormat /*aFormat*/, RMobileUssdMessaging::TMobileUssdMessageType /*aType*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageComp()
 *
 * @param aData The data to send in the message.
 * @param aDcs The data coding scheme of the message.
 * @param aFormat The message format.
 * @param aType The message type.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSendUssdMessageReqL

TInt CLtsySupplementaryServicesHandler::HandleSendUssdMessageNoFdnCheckReqL(const TDesC8& /*aData*/, TUint8 /*aDcs*/, 
		RMobileUssdMessaging::TMobileUssdDataFormat /*aFormat*/, RMobileUssdMessaging::TMobileUssdMessageType /*aType*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp()
 *
 * @param aData The data to send in the message.
 * @param aDcs The data coding scheme of the message.
 * @param aFormat The message format.
 * @param aType The message type.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSendUssdMessageNoFdnCheckReqL

TInt CLtsySupplementaryServicesHandler::HandleSendUssdReleaseReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdReleaseComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleSendUssdReleaseReqL


TInt CLtsySupplementaryServicesHandler::HandleClearBlacklistReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSupplementaryServicesClearBlacklistComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySupplementaryServicesHandler::HandleClearBlacklistReqL





