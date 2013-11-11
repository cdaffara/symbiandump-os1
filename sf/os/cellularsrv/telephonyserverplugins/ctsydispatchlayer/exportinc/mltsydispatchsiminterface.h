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
// the Licensee LTSY relating to Sim related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHSIMINTERFACE_H_
#define MLTSYDISPATCHSIMINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>
#include <etelmm.h>
#include <ctsy/rmmcustomapi.h>


class MLtsyDispatchSimGetApnControlListServiceStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetApnControlListServiceStatusApiId = KDispatchSimFuncUnitId + 1;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetAPNControlListServiceStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetApnControlListServiceStatusComp()
	 *
	 * Implementation of this interface should get the current Apn Control List Service status from the LTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetApnControlListServiceStatusReqL() = 0;

	}; // class MLtsyDispatchSimGetApnControlListServiceStatus



class MLtsyDispatchSimDeleteApnName : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimDeleteApnNameApiId = KDispatchSimFuncUnitId + 2;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneDeleteAPNName
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimDeleteApnNameComp()
	 *
	 * Implementation of this interface should delete the given entry from the APN control list.
	 *
	 * @param aIndex The index of the entry to delete.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleDeleteApnNameReqL(TUint32 aIndex) = 0;

	}; // class MLtsyDispatchSimDeleteApnName



class MLtsyDispatchSimEnumerateApnEntries : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimEnumerateApnEntriesApiId = KDispatchSimFuncUnitId + 3;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneEnumerateAPNEntries
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimEnumerateApnEntriesComp()
	 *
	 * Implementation of this interface should enumerate the available APN names in the Access Control list.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleEnumerateApnEntriesReqL() = 0;

	}; // class MLtsyDispatchSimEnumerateApnEntries



class MLtsyDispatchSimChangeSecurityCode : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimChangeSecurityCodeApiId = KDispatchSimFuncUnitId + 4;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneChangeSecurityCode
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimChangeSecurityCodeComp()
	 *
	 * Implementation of this interface should allow a client to change a security code.
	 *
	 * @param aSecCode The security code to be changed.
	 * @param aPasswds The old and the new values of the security code.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleChangeSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode aSecCode,
			   								  const RMobilePhone::TMobilePhonePasswordChangeV1& aPasswds) = 0;

	}; // class MLtsyDispatchSimChangeSecurityCode



class MLtsyDispatchSimSetFdnSetting : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSetFdnSettingApiId = KDispatchSimFuncUnitId + 5;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetFdnSetting
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSetFdnSettingComp()
	 *
	 * Implementation of this interface should activate or deactivate the Fixed Dialling Numbers (FDN) service
	 * PIN2 must have been verified prior to changing the Fixed Dialling Numbers setting.
	 *
	 * @param aFdnSetting The FDN setting value.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetFdnSettingReqL(RMobilePhone::TMobilePhoneFdnSetting aFdnSetting) = 0;

	}; // class MLtsyDispatchSimSetFdnSetting



class MLtsyDispatchSimGetCustomerServiceProfile : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetCustomerServiceProfileApiId = KDispatchSimFuncUnitId + 6;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetCustomerServiceProfile
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetCustomerServiceProfileComp()
	 *
	 * Implementation of this interface should retrieve the whole Customer Service Profile file from LTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetCustomerServiceProfileReqL() = 0;

	}; // class MLtsyDispatchSimGetCustomerServiceProfile



class MLtsyDispatchSimGetSubscriberId : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetSubscriberIdApiId = KDispatchSimFuncUnitId + 7;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetSubscriberId
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetSubscriberIdComp()
	 *
	 * Implementation of this interface should get the subscriber's identity as described by its IMSI from the LTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSubscriberIdReqL() = 0;

	}; // class MLtsyDispatchSimGetSubscriberId



class MLtsyDispatchSimAppendApnName : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimAppendApnNameApiId = KDispatchSimFuncUnitId + 8;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneAppendAPNName
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimAppendApnNameComp()
	 *
	 * Implementation of this interface should append the given entry to the end of the APN control list.
	 *
	 * @param aEntry The entry to append to the APN list.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleAppendApnNameReqL(const RMobilePhone::TAPNEntryV3& aEntry) = 0;

	}; // class MLtsyDispatchSimAppendApnName



class MLtsyDispatchSimGetActiveIccApplicationType : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetActiveIccApplicationTypeApiId = KDispatchSimFuncUnitId + 9;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsySimGetICCType
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetActiveIccApplicationTypeComp()
	 *
	 * This handler is called by the CTSY to retrieve the current active ICC application type.
	 * Types are defined in MLtsyDispatchSimGetActiveIccApplicationType::TIccType.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	
	enum TIccType
	    {
	    EIccTypeSim2GGsm,
	    EIccTypeSim3G,
	    EIccTypeSimUnknown
	    };
	
	virtual TInt HandleGetActiveIccApplicationTypeReqL() = 0;

	}; // class MLtsyDispatchSimGetActiveIccApplicationType



class MLtsyDispatchSimSetIccMessageWaitingIndicators : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId = KDispatchSimFuncUnitId + 10;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetIccMessageWaitingIndicators
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSetIccMessageWaitingIndicatorsComp()
	 *
	 * Implementation of this interface should set the message waiting indicators on the current ICC.
	 *
	 * @param aIndicators The message waiting indicators to set.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetIccMessageWaitingIndicatorsReqL(const RMobilePhone::TMobilePhoneMessageWaitingV1& aIndicators) = 0;

	}; // class MLtsyDispatchSimSetIccMessageWaitingIndicators



class MLtsyDispatchSimSetApnControlListServiceStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSetApnControlListServiceStatusApiId = KDispatchSimFuncUnitId + 11;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneSetAPNControlListServiceStatus
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSetApnControlListServiceStatusComp()
	 *
	 * Implementation of this interface should set the status of the APN Control List Service.
	 *
	 * @param aStatus The service status to set.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetApnControlListServiceStatusReqL(RMobilePhone::TAPNControlListServiceStatus aStatus) = 0;

	}; // class MLtsyDispatchSimSetApnControlListServiceStatus



class MLtsyDispatchSimGetApnName : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetApnNameApiId = KDispatchSimFuncUnitId + 12;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetAPNname
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetApnNameComp()
	 *
	 * Implementation of this interface should get the APN entry from the APN list at the index specified.
	 *
	 * @param aIndex The index of the APN entry to get from LTSY.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetApnNameReqL(TUint32 aIndex) = 0;

	}; // class MLtsyDispatchSimGetApnName



class MLtsyDispatchSimSimRefreshDone : public MLtsyDispatchInterface
	{
public:
	
	static const TInt KLtsyDispatchSimSimRefreshDoneApiId = KDispatchSimFuncUnitId + 13;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMmTsySimRefreshDoneIPC
	 * request from the CTSY.
	 *
	 * It is a request call that has no complete.
	 *
	 *
	 * Implementation of this interface indicates to the LTSY the SIM refresh has finished.
	 *
	 *
	 * @param aError An error code relating to the result of the refresh.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSimRefreshDoneReqL(TInt aError) = 0;

	}; // class MLtsyDispatchSimSimRefreshDone

class MLtsyDispatchSimGetServiceTable : public MLtsyDispatchInterface
	{
public:
	
	static const TInt KLtsyDispatchSimGetServiceTableApiId = KDispatchSimFuncUnitId + 14;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetServiceTable
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackSimGetServiceTableComp()
	 *
	 * Implementation of this interface should read the EFust or EFsst service tables as specified by aServiceTable.
	 *
	 * @param aServiceTable Specifies the service table to retrieve.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetServiceTableReqL(RMobilePhone::TMobilePhoneServiceTable aServiceTable) = 0;

	}; // class MLtsyDispatchSimGetServiceTable

class MLtsyDispatchSimGetIccMessageWaitingIndicators : public MLtsyDispatchInterface
	{
public:
	
	static const TInt KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId = KDispatchSimFuncUnitId + 15;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the EMobilePhoneGetIccMessageWaitingIndicators
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking CCtsyDispatcherCallback::CallbackSimGetIccMessageWaitingIndicatorsComp()
	 *
	 * Implementation of this interface should get the ICC message waiting indicators from the (U)SIM field EFmwis.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetIccMessageWaitingIndicatorsReqL() = 0;

	}; // class MLtsyDispatcSimGetIccMessageWaitingIndicators

class MLtsyDispatchSimSimLockActivate : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSimLockActivateApiId = KDispatchSimFuncUnitId + 16;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSimLockActivateIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSimLockActivateComp()
	 * 
	 * Implementation of this interface should send a SIM Lock Activation request.
	 *
	 * @param aPassword The password used for the SIM Lock activation.
	 * @param aLockNumber The lock number that defines SIM Lock types.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSimLockActivateReqL(const TDesC& aPassword, RMmCustomAPI::TLockNumber aLockNumber) = 0;

	}; // class MLtsyDispatchSimSimLockActivate


class MLtsyDispatchSimSimLockDeActivate : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSimLockDeActivateApiId = KDispatchSimFuncUnitId + 17;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSimLockDeActivateIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSimLockDeActivateComp()
	 * 
	 * Implementation of this interface should send a SIM Lock Deactivation request. 
	 * 
	 * @param aPassword The password used for the SIM Lock deactivation.
	 * @param aLockNumber The lock number that defines SIM Lock types.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSimLockDeActivateReqL(const TDesC& aPassword, RMmCustomAPI::TLockNumber aLockNumber) = 0;

	}; // class MLtsyDispatchSimSimLockDeActivate


class MLtsyDispatchSimGetAnswerToReset : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetAnswerToResetApiId = KDispatchSimFuncUnitId + 18;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetATRIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetAnswerToResetComp()
	 * 
	 * Implementation of this interface should retrieve the answer to reset. 
	 *
	 * @param aAnswerToReset The answer to reset information which contains details of the request.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetAnswerToResetReqL(const TDesC8& aAnswerToReset) = 0;

	}; // class MLtsyDispatchSimGetAnswerToReset


class MLtsyDispatchSimGetSimCardReaderStatus : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetSimCardReaderStatusApiId = KDispatchSimFuncUnitId + 19;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetSimCardReaderStatusIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetSimCardReaderStatusComp()
	 * 
	 * Implementation of this interface should retrieve the card reader status.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSimCardReaderStatusReqL() = 0;

	}; // class MLtsyDispatchSimGetSimCardReaderStatus


class MLtsyDispatchSimGetSimAuthenticationEapSimData : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId = KDispatchSimFuncUnitId + 20;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetSimAuthenticationDataIPC for 
 	 * Extensible Authentication Protocol Subscriber Identity Module authentication method
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationEapSimDataComp()
	 * 
	 * Implementation of this interface should retrieve Sim Authentication Data for EapSim authentication method.
	 *
	 * @param aRandomParameters The random parameters from the client.
	 * @param aRFStateInfo The RF state info.
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSimAuthenticationEapSimDataReqL(const TDesC8& aRandomParameters, TInt aRFStateInfo) = 0;

	}; // class MLtsyDispatchSimGetSimAuthenticationEapSimData

class MLtsyDispatchSimGetSimAuthenticationEapAkaData : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId = KDispatchSimFuncUnitId + 21;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomGetSimAuthenticationDataIPC for 
	 * Extensible Authentication Protocol UMTS Authentication and Key Agreement authentication method
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationEapAkaDataComp()
	 * 
	 * Implementation of this interface should retrieve Sim Authentication Data for EapAka authentication method.
	 *
	 * @param aRandomParameters The random parameters from the client.
	 * @param aAUTN The AUTN parameter. AUTN is an authentication value generated by
	 *        the Authentication Centre, which, together with the random parameters, authenticates the server to the peer, 128 bits.
	 * @param aRFStateInfo The RF state info.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSimAuthenticationEapAkaDataReqL(const TDesC8& aRandomParameters, const TDesC8& aAUTN, TInt aRFStateInfo) = 0;

	}; // class MLtsyDispatchSimGetSimAuthenticationEapAkaData

class MLtsyDispatchSimPowerSimOff : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimPowerSimOffApiId = KDispatchSimFuncUnitId + 22;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomPowerSimOffIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimPowerSimOffComp()
	 * 
	 * Implementation of this interface should send a Power SIM Off request to the APDU server.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::PowerSimOff()
	 */
	virtual TInt HandlePowerSimOffReqL() = 0;

	}; // class MLtsyDispatchSimPowerSimOff


class MLtsyDispatchSimPowerSimOn : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimPowerSimOnApiId = KDispatchSimFuncUnitId + 23;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomPowerSimOnIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimPowerSimOnComp()
	 * 
	 * Implementation of this interface should send a Power SIM On request to the APDU server.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::PowerSimOn()
	 */
	virtual TInt HandlePowerSimOnReqL() = 0;

	}; // class MLtsyDispatchSimPowerSimOn


class MLtsyDispatchSimReadSimFile : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimReadSimFileApiId = KDispatchSimFuncUnitId + 24;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomReadSimFileIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimReadSimFileComp()
	 * 
	 * Implementation of this interface should request to read full or partial contents of a 
	 * given SIM file. The path, size and offset for the requested SIM file are provided.
	 * 
	 *
	 * @param aPath The absolute file path of the SIM file in the file system of the SIM.
	 * see ETSI TS 131 102 Under "Files of USIM"
	 * @param aOffset The value of the Offset, the offset is related with the type of SIM file being read.
	 * @param aSize The amount of bytes to be read from the SIM file.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::ReadSimFile()
	 */
	virtual TInt HandleReadSimFileReqL(const TDesC8& aPath, TUint16 aOffset, TUint16 aSize) = 0;

	}; // class MLtsyDispatchSimReadSimFile


class MLtsyDispatchSimSendApduRequest : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSendApduRequestApiId = KDispatchSimFuncUnitId + 25;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSendAPDUReqIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSendApduRequestComp()
	 * 
	 * Implementation of this interface should pass the send APDU request to the baseband.
	 * 
	 * The service type, card reader number and application type is the address information 
	 * and the data is an envelope. The client side and baseband are responsible for the 
	 * consistency of the data buffer's content (i.e. they have to have an agreement on the contents).
	 * 
	 *
	 * @param aServiceType The value for the service type of the request.
	 * @param aCardReaderNumber The value for the card reader number of the request.
	 * @param aApplicationType The value for the application type of the request.
	 * @param aCommandData The transparent data to be conveyed to the Baseband.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::SendAPDUReq()
	 */
	virtual TInt HandleSimSendApduRequestReqL(TUint8 aServiceType, TUint8 aCardReaderNumber, TUint8 aApplicationType, const TDesC8& aCommandData) = 0;

	}; // class MLtsyDispatchSimSendApduRequest

class MLtsyDispatchSimSendApduRequestV2 : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSendApduRequestV2ApiId = KDispatchSimFuncUnitId + 26;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSendAPDUReqV2IPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSendApduRequestV2Comp()
	 * 
	 * Implementation of this interface should pass the send APDU request to the baseband.
	 * 
	 * The client side and baseband are responsible for the consistency of the data buffer's content 
	 * (i.e. they have to have an agreement on the contents).
	 * 
	 *
	 * @param aCardReaderId The value for the card reader Id of the request.
	 * @param aCommandData The transparent data to be conveyed to the Baseband.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::SendAPDUReq()
	 */
	virtual TInt HandleSimSendApduRequestV2ReqL(TUint8 aCardReaderId, const TDesC8& aCommandData) = 0;

	}; // class MLtsyDispatchSimSendApduRequest

class MLtsyDispatchSimSimWarmReset : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSimWarmResetApiId = KDispatchSimFuncUnitId + 27;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSimWarmResetIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSimWarmResetComp()
	 * 
	 * Implementation of this interface should handle the reqest for a SIM warm reset.
	 * 
	 * See ETSI TS 102 221 under "Reset procedures"
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::SimWarmReset()
	 */
	virtual TInt HandleSimWarmResetReqL() = 0;

	}; // class MLtsyDispatchSimSimWarmReset


class MLtsyDispatchSimSetSimMessageStatusRead : virtual MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSimSetSimMessageStatusReadApiId = KDispatchSimFuncUnitId + 28;

	/**
	 * The CTSY Dispatcher shall invoke this function on receiving the ECustomSetSimMessageStatusReadIPC
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSimSetSimMessageStatusReadComp()
	 * 
	 * No error code is returned to the ETel client - if no SMS message with matching timestamp is identified
	 * on the SIM then this error will be silently ignored.
	 *
	 * @param aTimestamp timestamp that uniquely identifies a particular message in the message store.
	 * @param aTimezoneDiff the difference, expressed in quarters of an hour, between the service centre local time and GMT
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RMmCustomAPI::SetSimMessageStatusRead()
	 */
	virtual TInt HandleSetSimMessageStatusReadReqL(const TTime& aTimestamp, TInt aTimezoneDiff) = 0;

	}; // class MLtsyDispatchSimSetSimMessageStatusRead


#endif /*MLTSYDISPATCHSIMINTERFACE_H_*/
