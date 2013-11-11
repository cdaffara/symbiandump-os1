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
// CLtsySimHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsysimhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsySimHandler::CLtsySimHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsySimHandler::~CLtsySimHandler()
	{
	}


CLtsySimHandler* CLtsySimHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySimHandler* self = new (ELeave) CLtsySimHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySimHandler* CLtsySimHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySimHandler* self=
			CLtsySimHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySimHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsySimHandler::ConstructL

TBool CLtsySimHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
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

void CLtsySimHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
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


TInt CLtsySimHandler::HandleGetApnControlListServiceStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetApnControlListServiceStatusComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetApnControlListServiceStatusReqL


TInt CLtsySimHandler::HandleDeleteApnNameReqL(TUint32 /*aIndex*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimDeleteApnNameComp()
 *
 * @param aIndex The index of the entry to delete.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleDeleteApnNameReqL


TInt CLtsySimHandler::HandleEnumerateApnEntriesReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimEnumerateApnEntriesComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleEnumerateApnEntriesReqL


TInt CLtsySimHandler::HandleChangeSecurityCodeReqL(RMobilePhone::TMobilePhoneSecurityCode /*aSecCode*/,
												   const RMobilePhone::TMobilePhonePasswordChangeV1& /*aPasswds*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimChangeSecurityCodeComp()
 *
 * @param aSecCode The security code to be changed.
 * @param aPasswds The old and the new values of the security code.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleChangeSecurityCodeReqL


TInt CLtsySimHandler::HandleSetFdnSettingReqL(RMobilePhone::TMobilePhoneFdnSetting /*aFdnSetting*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSetFdnSettingComp()
 *
 * @param aFdnSetting The FDN setting.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSetFdnSettingReqL


TInt CLtsySimHandler::HandleGetCustomerServiceProfileReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetCustomerServiceProfileComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetCustomerServiceProfileReqL


TInt CLtsySimHandler::HandleGetSubscriberIdReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetSubscriberIdComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetSubscriberIdReqL


TInt CLtsySimHandler::HandleAppendApnNameReqL(const RMobilePhone::TAPNEntryV3& /*aEntry*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimAppendApnNameComp()
 *
 * @param aEntry The entry to append to the APN list.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleAppendApnNameReqL


TInt CLtsySimHandler::HandleGetActiveIccApplicationTypeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetActiveIccApplicationTypeComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetActiveIccApplicationTypeReqL


TInt CLtsySimHandler::HandleSetIccMessageWaitingIndicatorsReqL(const RMobilePhone::TMobilePhoneMessageWaitingV1& /*aIndicators*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSetIccMessageWaitingIndicatorsComp()
 *
 * @param aIndicators The message waiting indicators to set.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSetIccMessageWaitingIndicatorsReqL


TInt CLtsySimHandler::HandleSetApnControlListServiceStatusReqL(RMobilePhone::TAPNControlListServiceStatus /*aStatus*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSetApnControlListServiceStatusComp()
 *
 * @param aStatus The service status to set.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSetApnControlListServiceStatusReqL


TInt CLtsySimHandler::HandleGetApnNameReqL(TUint32 /*aIndex*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetApnNameComp()
 *
 * @param aIndex The index of the APN entry to get from LTSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetApnNameReqL


TInt CLtsySimHandler::HandleSimRefreshDoneReqL(TInt /*aError*/)
/**
 * This request has no associated callback.
 *
 * @param aError The error which has occured during a SIM refresh, KErrNone if no error.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimRefreshDoneSyncL

TInt CLtsySimHandler::HandleGetServiceTableReqL(
							RMobilePhone::TMobilePhoneServiceTable /*aServiceTable*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetServiceTableComp()
 * 
 * @param aServiceTable Service table to get information about.
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......


	
	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetServiceTableReqL

TInt CLtsySimHandler::HandleGetIccMessageWaitingIndicatorsReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetIccMessageWaitingIndicatorsComp()
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise. 
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	//TO DO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetIccMessageWaitingIndicatorsReqL

TInt CLtsySimHandler::HandleSimLockActivateReqL(const TDesC& /*aPassword*/, RMmCustomAPI::TLockNumber /*aLockNumber*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSimLockActivateComp()
 *
 * @param aPassword The password used for the SIM Lock activation.
 * @param aLockNumber The lock number that defines SIM Lock types.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimLockActivateReqL


TInt CLtsySimHandler::HandleSimLockDeActivateReqL(const TDesC& /*aPassword*/, RMmCustomAPI::TLockNumber /*aLockNumber*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSimLockDeActivateComp()
 *
 * @param aPassword The password used for the SIM Lock deactivation.
 * @param aLockNumber The lock number that defines SIM Lock types.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimLockDeActivateReqL


TInt CLtsySimHandler::HandleGetAnswerToResetReqL(const TDesC8& /*aAnswerToReset*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetAnswerToResetComp()
 *
 * @param aAnswerToReset The answer to reset information which contains details of the request.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetAnswerToResetReqL


TInt CLtsySimHandler::HandleGetSimCardReaderStatusReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetSimCardReaderStatusComp()
 *
 * @param aSimCardReaderStatus The sim card reader status information which contains details of the request.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	

	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetSimCardReaderStatusReqL


TInt CLtsySimHandler::HandleGetSimAuthenticationEapSimDataReqL(const TDesC8& /*aRandomParameters*/, TInt /*aRFStateInfo*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationEapSimDataComp()
 *
 * @param aRandomParameters The random parameters from the client.
 * @param aRFStateInfo The RF state info.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetSimAuthenticationEapSimDataReqL

TInt CLtsySimHandler::HandleGetSimAuthenticationEapAkaDataReqL(const TDesC8& /*aRandomParameters*/, const TDesC8& /*aAUTN*/, TInt /*aRFStateInfo*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimGetSimAuthenticationEapAkaDataComp()
 *
 * @param aRandomParameters The random parameters from the client.
 * @param aAUTN The AUTN parameter. AUTN is an authentication value generated by
 *        the Authentication Centre, which, together with the random parameters, authenticates the server to the peer, 128 bits.
 * @param aRFStateInfo The RF state info.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	//TO DO: Add implementation here.......
	


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleGetSimAuthenticationEapAkaDataReqL


TInt CLtsySimHandler::HandlePowerSimOffReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimPowerSimOffComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandlePowerSimOffReqL


TInt CLtsySimHandler::HandlePowerSimOnReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimPowerSimOnComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandlePowerSimOnReqL

TInt CLtsySimHandler::HandleReadSimFileReqL(const TDesC8& /*aPath*/, TUint16 /*aOffset*/, TUint16 /*aSize*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimReadSimFileComp()
 *
 * @param aPath The absolute file path of the SIM file in the file system of the SIM.
 * see ETSI TS 131 102 Under "Files of USIM"
 * @param aOffset The value of the Offset, the offset is related with the type of SIM file being read.
 * @param aSize The amount of bytes to be read from the SIM file.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleReadSimFileReqL

TInt CLtsySimHandler::HandleSimSendApduRequestReqL(TUint8 /*aServiceType*/, TUint8 /*aCardReaderNumber*/, TUint8 /*aApplicationType*/, const TDesC8& /*aCommandData*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSendApduRequestComp()
 *
 * @param aServiceType The value for the service type of the request.
 * @param aCardReaderNumber The value for the card reader number of the request.
 * @param aApplicationType The value for the application type of the request.
 * @param aCommandData The transparent data to be conveyed to the Baseband.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimSendApduRequestReqL

TInt CLtsySimHandler::HandleSimSendApduRequestV2ReqL(TUint8 /*aCardReaderId*/, const TDesC8& /*aCommandData*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSendApduRequestV2Comp()
 *
 * @param aCardReaderId The value for the card reader Id of the request.
 * @param aCommandData The transparent data to be conveyed to the Baseband.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimSendApduRequestV2ReqL


TInt CLtsySimHandler::HandleSimWarmResetReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSimWarmResetComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSimWarmResetReqL


TInt CLtsySimHandler::HandleSetSimMessageStatusReadReqL(const TTime& /*aTimestamp*/, TInt /*aTimezoneDiff*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSimSetSimMessageStatusRead()
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
 **/
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySimHandler::HandleSetSimMessageStatusReadReqL
