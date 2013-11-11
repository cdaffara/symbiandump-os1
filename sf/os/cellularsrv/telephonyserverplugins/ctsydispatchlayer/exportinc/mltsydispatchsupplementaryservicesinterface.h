// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to SupplementaryServices related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHSUPPLEMENTARYSERVICESINTERFACE_H_
#define MLTSYDISPATCHSUPPLEMENTARYSERVICESINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>


class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheckApiId = KDispatchSupplementaryServicesFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSendNetworkServiceRequestNoFdnCheck
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestNoFdnCheckComp()
	 * 
	 * Implementation of this interface should send the request to the network without FDN check.
	 *
	 *
	 * @param aRequest The supplementary services string that was entered by the user and need to be send to the network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendNetworkServiceRequestNoFdnCheckReqL(const TDesC& aRequest) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequestNoFdnCheck



class MLtsyDispatchSupplementaryServicesGetCallWaitingStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesGetCallWaitingStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetWaitingStatusPhase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallWaitingStatusComp()
	 *
	 * Implementation of this interface should get the call waiting list.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCallWaitingStatusReqL() = 0;

	}; // class MLtsyDispatchSupplementaryServicesGetCallWaitingStatus



class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSendNetworkServiceRequestApiId = KDispatchSupplementaryServicesFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSendNetworkServiceRequest
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendNetworkServiceRequestComp()
	 * 
	 * Implementation of this interface should send the request to the network after FDN check.
	 *
	 *
	 * @param aRequest The supplementary services string that was entered by the user and need to be send to the network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendNetworkServiceRequestReqL(const TDesC& aRequest) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSendNetworkServiceRequest



class MLtsyDispatchSupplementaryServicesGetCallBarringStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesGetCallBarringStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetBarringStatusPhase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallBarringStatusComp()
	 * 
	 * Implementation of this interface should get the call barring list.
	 *
	 *
	 * @param aCondition The barring conditions to interrogate.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition aCondition) = 0;

	}; // class MLtsyDispatchSupplementaryServicesGetCallBarringStatus



class MLtsyDispatchSupplementaryServicesSetCallBarringStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSetCallBarringStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetCallBarringStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallBarringStatusComp()
	 * 
	 * Implementation of this interface should set the call barring status for a call barring condition.
	 *
	 *
	 * @param aCondition The call barring condition to set.
	 * @param aAction The call barring action.
	 * @param aGroup The call barring service group.
	 * @param aPassword The password of the call barring service.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetCallBarringStatusReqL(RMobilePhone::TMobilePhoneCBCondition aCondition,
        RMobilePhone::TMobilePhoneServiceAction aAction,
        RMobilePhone::TMobileService aGroup,
        const TDesC& aPassword) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSetCallBarringStatus



class MLtsyDispatchSupplementaryServicesSetCallForwardingStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSetCallForwardingStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetCallForwardingStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallForwardingStatusComp()
	 * 
	 * Implementation of this interface should sets a new call forwarding status and/or registered 
	 * information for the call forwarding condition.
	 *
	 *
	 * @param aCondition The condition of the new call forwarding setting.
	 * @param aServiceGroup The basic service group to apply the CF status change to.
	 * @param aAction The action to take.
	 * @param aNumber The forwarding number.
	 * @param aTimeout The time out when aCondition is ECallForwardingNoReply.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition aCondition,
			RMobilePhone::TMobileService aServiceGroup,
			RMobilePhone::TMobilePhoneServiceAction aAction,
			const TDesC& aNumber,
			TInt aTimeout) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSetCallForwardingStatus



class MLtsyDispatchSupplementaryServicesSetCallWaitingStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSetCallWaitingStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetCallWaitingStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetCallWaitingStatusComp()
	 * 
	 * Implementation of this interface should set the call waiting status.
	 *
	 *
	 * @param aServiceGroup The service group to change.
	 * @param aAction The action to do with that service group.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetCallWaitingStatusReqL(RMobilePhone::TMobileService aServiceGroup, RMobilePhone::TMobilePhoneServiceAction aAction) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSetCallWaitingStatus



class MLtsyDispatchSupplementaryServicesSetSsPassword : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSetSsPasswordApiId = KDispatchSupplementaryServicesFuncUnitId + 8;

	enum TServiceType
		{
		EAllSupplementaryServices = 0,
		ECallBarring = 330
		};
	
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetSSPassword
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSetSsPasswordComp()
	 * 
	 * Implementation of this interface should set the password of the call barring or common Supplementary Services password.
	 *
	 *
	 * @param aOldPassword The old password.
	 * @param aNewPassword The new password.
	 * @param aService The service.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetSsPasswordReqL(const TDesC& aOldPassword,
			const TDesC& aNewPassword,
			TServiceType aService) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSetSsPassword



class MLtsyDispatchSupplementaryServicesGetCallForwardingStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesGetCallForwardingStatusApiId = KDispatchSupplementaryServicesFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetCallForwardingStatusPhase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesGetCallForwardingStatusComp()
	 * 
	 * Implementation of this interface should get the call forwarding list.
	 *
	 * @param aCondition The call forwarding condition.
	 * @param aService The service group.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCallForwardingStatusReqL(RMobilePhone::TMobilePhoneCFCondition aCondition, 
			RMobilePhone::TMobileService aService) = 0;

	}; // class MLtsyDispatchSupplementaryServicesGetCallForwardingStatus



class MLtsyDispatchSupplementaryServicesSendUssdMessage : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSendUssdMessageApiId = KDispatchSupplementaryServicesFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileUssdMessagingSendMessage
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageComp()
	 * 
	 * Implementation of this interface should send an USSD message to the network.
	 *
	 *
	 * @param aData The actual data to send in the message.
	 * @param aDcs The data coding scheme of the message.
	 * @param aFormat The message format.
	 * @param aType The message type (RMobileUssdMessaging::EUssdMORequest, RMobileUssdMessaging::EUssdMOReply 
	 * or RMobileUssdMessaging::EUssdMOAcknowledgement).
	 *
	 * @return KErrNone on success, KErrGsmSSUssdBusy if the latest USSD session was not yet released, 
	 * otherwise another error code indicating the failure.
	 */
	virtual TInt HandleSendUssdMessageReqL(const TDesC8& aData, TUint8 aDcs, 
			RMobileUssdMessaging::TMobileUssdDataFormat aFormat, RMobileUssdMessaging::TMobileUssdMessageType aType) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSendUssdMessage


class MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheckApiId = KDispatchSupplementaryServicesFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileUssdMessagingSendMessageNoFdnCheck
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdMessageNoFdnCheckComp()
	 * 
	 * Implementation of this interface should send an USSD message to the network without initially checking FDN lsit.
	 *
	 *
	 * @param aData The actual data to send in the message.
	 * @param aDcs The data coding scheme of the message.
	 * @param aFormat The message format.
	 * @param aType The message type (RMobileUssdMessaging::EUssdMORequest, RMobileUssdMessaging::EUssdMOReply 
	 * or RMobileUssdMessaging::EUssdMOAcknowledgement).
	 *
	 * @return KErrNone on success, KErrGsmSSUssdBusy if the latest USSD session was not yet released, 
	 * otherwise another error code indicating the failure.
	 */
	virtual TInt HandleSendUssdMessageNoFdnCheckReqL(const TDesC8& aData, TUint8 aDcs, 
			RMobileUssdMessaging::TMobileUssdDataFormat aFormat, RMobileUssdMessaging::TMobileUssdMessageType aType) = 0;

	}; // class MLtsyDispatchSupplementaryServicesSendUssdMessageNoFdnCheck


class MLtsyDispatchSupplementaryServicesSendUssdRelease : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesSendUssdReleaseApiId = KDispatchSupplementaryServicesFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileUssdMessagingSendRelease
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesSendUssdReleaseComp()
	 *
	 * Implementation of this interface should send a USSD release message to the network.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendUssdReleaseReqL() = 0;

	}; // class MLtsyDispatchSupplementaryServicesSendUssdRelease



class MLtsyDispatchSupplementaryServicesClearBlacklist : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSupplementaryServicesClearBlacklistApiId = KDispatchSupplementaryServicesFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomClearCallBlackListIPC
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSupplementaryServicesClearBlacklistComp()
	 * 
	 * Implementation of this interface should clear the blacklist.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the failure.
	 */
	virtual TInt HandleClearBlacklistReqL() = 0;

	}; // class MLtsyDispatchSupplementaryServicesClearBlacklist


#endif /*MLTSYDISPATCHSUPPLEMENTARYSERVICESINTERFACE_H_*/
