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
// the Licensee LTSY relating to Sms related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHSMSINTERFACE_H_
#define MLTSYDISPATCHSMSINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>

namespace DispatcherSim
	{
	
	/**
	 * This namespace contains all types relating to the Sim dispatcher.
	 */

	/** SMS parameters */
	struct TSmsParameters
	    {
	    /*
	     * LocationNumber
	     */    
	    TUint8 iLocationNumber;

	    /*
	     * Parameter Indicator
	     */    
	    TUint8 iParameterIndicator;

	    /*
	     * ProtocolId
	     */    
	    TUint8 iProtocolId;

	    /*
	     * Data Coding Scheme
	     */    
	    TUint8 iDataCodingScheme;

	    /*
	     * Validity Period
	     */    
	    TUint8 iValidityPeriod;

	    /*
	     * MobileSc TON
	     */    
	    RMobilePhone::TMobileTON iMobileScTON;

	    /*
	     * MobileSc NPI
	     */    
	    RMobilePhone::TMobileNPI iMobileScNPI;

	     /*
	     * Mobile De TON
	     */    
	    RMobilePhone::TMobileTON iMobileDeTON;

	    /*
	     * Mobile De NPI
	     */    
	    RMobilePhone::TMobileNPI iMobileDeNPI;

	    /*
	     * Alpha Tag Present
	     */    
	    TBool iAlphaTagPresent;
	    };	
	}

class MLtsyDispatchSmsSendSatSms : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsSendSatSmsApiId = KDispatchSmsFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsySmsSendSatMessage
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp()
	 *
	 * Implementation of this interface should handle a request to send a SMS message initiated from a client
	 * side RSat::SendMessageNoLogging() call.
	 *
	 * @param aDestination The mobile telephone number.
	 * @param aSmsTpdu The SMS TPDU.
	 * @param aDataFormat The SMS data format.
	 * @param aGsmServiceCentre The service centre number.
	 * @param aMore  This indicates whether the client is going to send another SMS immediately 
	                 after this one.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendSatSmsReqL( const TDesC8& aSmsTpdu,
										   RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
										   const RMobilePhone::TMobileAddress& aGsmServiceCentre,
										   TBool aMoreMessages
										  ) = 0;
	
	}; // class MLtsyDispatchSmsSendSatSms



class MLtsyDispatchSmsGetSmsStoreInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsGetSmsStoreInfoApiId = KDispatchSmsFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingGetMessageStoreInfo
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp()
	 *
	 * Implementation of this interface should retrieve the current Sms store information.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileSmsMessaging::GetMessageStoreInfo
	 */
	virtual TInt HandleGetSmsStoreInfoReqL() = 0;

	}; // class MLtsyDispatchSmsGetSmsStoreInfo



class MLtsyDispatchSmsGetSmspList : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsGetSmspListApiId = KDispatchSmsFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingGetSmspListPhase1
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsGetSmspListComp()
	 *
	 * Implementation of this interface should request to read the SMS parameter list from the SIM's SMSP store.
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see CMobilePhoneSmspList()
	 * @see CRetrieveMobilePhoneSmspList()
	 */
	virtual TInt HandleGetSmspListReqL() = 0;

	}; // class MLtsyDispatchSmsGetSmspList



class MLtsyDispatchSmsNackSmsStored : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsNackSmsStoredApiId = KDispatchSmsFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingNackSmsStored
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp()
	 * 
	 * Implementation of this interface should accept this negative acknowledgment: when the client receives an unstored SMS,
	 * it will use this member function to return a negative acknowledgment if it fails to decode and/or store that SMS.
	 * This TPDU will contain the TP error cause that prevents the client from acknowledging the SMS. 
	 * The client should also provide the RP error cause by placing it in the aRpCause parameter.
	 *
	 * @param aTpdu Holds the TPDU defined for a SMS-DELIVER-REPORT (for RP-ERROR) in GSM 03.40.
	 * @param aRpCause Contain one of the MS related extended error codes.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileSmsMessaging::NackSmsStored()
	 */
	virtual TInt HandleNackSmsStoredReqL(const TDesC8& aTpdu, TInt aRpCause) = 0;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingNackSmsStored
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp()
	 * 
	 * Implementation of this interface should accept this negative acknowledgment: when the client receives an unstored SMS,
	 * it will use this member function to return a negative acknowledgment if it fails to decode and/or store that SMS.
	 * The client should also provide the RP error cause by placing it in the aRpCause parameter.
	 *
	 * @param aRpCause Contain one of the MS related extended error codes.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileSmsMessaging::NackSmsStored()
	 */
	virtual TInt HandleNackSmsStoredReqL(TInt aRpCause) = 0;

	}; // class MLtsyDispatchSmsNackSmsStored



class MLtsyDispatchSmsAckSmsStored : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsAckSmsStoredApiId = KDispatchSmsFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingAckSmsStored
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp()
	 * 
	 * Implementation of this interface should accept the acknowledgment: when the client receives an unstored SMS, 
	 * it will trigger this function to return a positive acknowledgment if it manages to successfully decode and store that SMS.
	 * If the client wishes to send a SMS-DELIVER-REPORT in response to the received SMS-DELIVER , 
	 * then the aTpdu parameter will be used to hold the TPDU defined for a SMS-DELIVER-REPORT (for RP-ACK) in GSM 03.40. 
	 *
	 * 
	 * @param aTpdu Holds the TPDU defined for a SMS-DELIVER-REPORT (for RP-ACK) in GSM 03.40.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileSmsMessaging::AckSmsStored()
	 */
	virtual TInt HandleAckSmsStoredReqL(const TDesC8& aTpdu) = 0;
	
	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingAckSmsStored
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp()
	 * 
	 * Implementation of this interface should accept the acknowledgment: when the client receives an unstored SMS, 
	 * it will trigger this function to return a positive acknowledgment if it manages to successfully decode and store that SMS.
	 *
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMobileSmsMessaging::AckSmsStored()
	 */
	virtual TInt HandleAckSmsStoredReqL() = 0;

	}; // class MLtsyDispatchSmsAckSmsStored



class MLtsyDispatchSmsResumeSmsReception : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsResumeSmsReceptionApiId = KDispatchSmsFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingResumeSmsReception
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp()
	 *
	 * Implementation of this interface should allow a client to resume sms reception 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleResumeSmsReceptionReqL() = 0;

	}; // class MLtsyDispatchSmsResumeSmsReception


class MLtsyDispatchSmsSendSmsMessage : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsSendSmsMessageApiId = KDispatchSmsFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingSendMessage
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp()
	 * 
	 * Implementation of this interface should allow a client to send a SMS message 
	 * 
	 * @param aDestination The mobile telephone number.
	 * @param aSmsTpdu The SMS TPDU.
	 * @param aDataFormat The SMS data format.
	 * @param aGsmServiceCentre The service centre number.
	 * @param aMore  This indicates whether the client is going to send another SMS immediately 
	 *                after this one.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendSmsMessageReqL( const RMobilePhone::TMobileAddress& aDestination,
										   const TDesC8& aSmsTpdu,
										   RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
										   const RMobilePhone::TMobileAddress& aGsmServiceCentre,
										   TBool aMoreMessages
										  ) = 0;


	}; // class MLtsyDispatchSmsSendSmsMessage


class MLtsyDispatchSmsSendSmsMessageNoFdnCheck : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId = KDispatchSmsFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingSendMessageNoFdnCheck
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp()
	 * 
	 * Implementation of this interface should allow a client to send a SMS message
	 * 
	 *
	 * @param aDestination The mobile telephone number.
	 * @param aSmsTpdu The SMS TPDU.
	 * @param aDataFormat The SMS data format.
	 * @param aGsmServiceCentre The service centre number.
	 * @param aMore  This indicates whether the client is going to send another SMS immediately 
	                 after this one.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSendSmsMessageNoFdnCheckReqL( const RMobilePhone::TMobileAddress& aDestination,
												     const TDesC8& aSmsTpdu,
													 RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
													 const RMobilePhone::TMobileAddress& aGsmServiceCentre,
													 TBool aMoreMessages
												   ) = 0;

	}; // class MLtsyDispatchSmsSendSmsMessageNoFdnCheck


class MLtsyDispatchSmsSetMoSmsBearer : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsSetMoSmsBearerApiId = KDispatchSmsFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingSetMoSmsBearer
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp()
	 *
	 * Implementation of this interface should allow client to set SMS bearer
	 * 
	 * @param aBearer The SMS bearer.
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetMoSmsBearerReqL(RMobileSmsMessaging::TMobileSmsBearer aBearer) = 0;

	}; // class MLtsyDispatchSmsSetMoSmsBearer



class MLtsyDispatchSmsStoreSmspListEntry : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSmsStoreSmspListEntryApiId = KDispatchSmsFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobileSmsMessagingStoreSmspList
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSmsStoreSmspListComp()
	 * 
	 * Implementation of this interface should handle the request to store a SMSP entry 
	 * in the SIM's SMSP file
	 *
	 * @param aSmspEntry Defines a set of SMS parameters
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleStoreSmspListEntryReqL(const RMobileSmsMessaging::TMobileSmspEntryV1& aSmspEntry) = 0;

	}; // class MLtsyDispatchSmsStoreSmspList


#endif /*MLTSYDISPATCHSMSINTERFACE_H_*/
