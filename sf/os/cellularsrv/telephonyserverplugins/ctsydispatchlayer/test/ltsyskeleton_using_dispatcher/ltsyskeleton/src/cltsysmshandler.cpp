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
// cltsydispatchsmshandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>



#include "cltsysmshandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsySmsHandler::CLtsySmsHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsySmsHandler::~CLtsySmsHandler()
	{
	}


CLtsySmsHandler* CLtsySmsHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySmsHandler* self = new (ELeave) CLtsySmsHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySmsHandler* CLtsySmsHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySmsHandler* self=
			CLtsySmsHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySmsHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsySmsHandler::ConstructL

TBool CLtsySmsHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchSmsFuncUnitId, aDispatchApiId);
	
	/* uncomment when implementing for real LTSY
	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	*/
	}

void CLtsySmsHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchSmsFuncUnitId, aIdGroup, aIndIdBitMask);
	
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


TInt CLtsySmsHandler::HandleSendSatSmsReqL( const TDesC8& aSmsTpdu,
		   RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
		   const RMobilePhone::TMobileAddress& aGsmServiceCentre,
		   TBool aMoreMessages
		  )
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSendSatSmsComp()
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchSmsFuncUnitId, MLtsyDispatchSmsSendSatSms::KLtsyDispatchSmsSendSatSmsApiId,
									&aSmsTpdu,
									aMoreMessages,
									&aGsmServiceCentre,
									aDataFormat);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSatSmsReqL


TInt CLtsySmsHandler::HandleGetSmsStoreInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsGetSmsStoreInfoComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchSmsFuncUnitId, MLtsyDispatchSmsGetSmsStoreInfo::KLtsyDispatchSmsGetSmsStoreInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleGetSmsStoreInfoReqL


TInt CLtsySmsHandler::HandleGetSmspListReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsGetSmspListComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchSmsFuncUnitId, MLtsyDispatchSmsGetSmspList::KLtsyDispatchSmsGetSmspListApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleGetSmspListReqL


TInt CLtsySmsHandler::HandleNackSmsStoredReqL(const TDesC8& aTpdu, TInt aRpCause)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp()
 *
 * @param aTpdu Reference to TPDU data.
 * @param aRpCause RP cause.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, 
	                               KDispatchSmsFuncUnitId, 
	                               MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId,
	                               &aTpdu,
	                               aRpCause);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleNackSmsStoredReqL


TInt CLtsySmsHandler::HandleNackSmsStoredReqL(TInt aRpCause)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsNackSmsStoredComp()
 *
 * @param aRpCause RP cause.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, 
	                               KDispatchSmsFuncUnitId, 
	                               MLtsyDispatchSmsNackSmsStored::KLtsyDispatchSmsNackSmsStoredApiId,
	                               NULL,
	                               aRpCause);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleNackSmsStoredReqL


TInt CLtsySmsHandler::HandleAckSmsStoredReqL(const TDesC8& aTpdu)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp()
 *
 * @param aTpdu Contains the TPDU defined for a SMS-DELIVER-REPORT.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, 
			                       KDispatchSmsFuncUnitId, 
			                       MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId,
			                       &aTpdu);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleAckSmsStoredReqL

TInt CLtsySmsHandler::HandleAckSmsStoredReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsAckSmsStoredComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, 
			                       KDispatchSmsFuncUnitId, 
			                       MLtsyDispatchSmsAckSmsStored::KLtsyDispatchSmsAckSmsStoredApiId,
			                       NULL);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleAckSmsStoredReqL

TInt CLtsySmsHandler::HandleResumeSmsReceptionReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsResumeSmsReceptionComp()
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchSmsFuncUnitId, MLtsyDispatchSmsResumeSmsReception::KLtsyDispatchSmsResumeSmsReceptionApiId);

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
 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageComp()
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

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchSmsFuncUnitId, 
									MLtsyDispatchSmsSendSmsMessage::KLtsyDispatchSmsSendSmsMessageApiId,
			                        &aDestination,			                        
			                        &aSmsTpdu,
			                        aMoreMessages,
			                        &aGsmServiceCentre,
			                        aDataFormat								   
			                      );

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSmsMessageReqL

TInt CLtsySmsHandler::HandleSendSmsMessageNoFdnCheckReqL( const RMobilePhone::TMobileAddress& aDestination,
	     												  const TDesC8& aSmsTpdu,
	     												  RMobileSmsMessaging::TMobileSmsDataFormat aDataFormat,
	     												  const RMobilePhone::TMobileAddress& aGsmServiceCentre,
	     												  TBool aMoreMessages
	   													) 
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSendSmsMessageNoFdnCheckComp()
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

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ( ret, 
			                        KDispatchSmsFuncUnitId, 
			                        MLtsyDispatchSmsSendSmsMessageNoFdnCheck::KLtsyDispatchSmsSendSmsMessageNoFdnCheckApiId,
			                        &aDestination,			                        
			                        &aSmsTpdu,
			                        aMoreMessages,
			                        &aGsmServiceCentre,
			                        aDataFormat
			                       );

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSendSmsMessageNoFdnCheckReqL

TInt CLtsySmsHandler::HandleSetMoSmsBearerReqL(RMobileSmsMessaging::TMobileSmsBearer aBearer)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsSetMoSmsBearerComp()
 *
 * 
 * @param aBearer The SMS bearer.
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(	ret, 
									KDispatchSmsFuncUnitId, 
									MLtsyDispatchSmsSetMoSmsBearer::KLtsyDispatchSmsSetMoSmsBearerApiId,
									aBearer
									);

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleSetMoSmsBearerReqL


TInt CLtsySmsHandler::HandleStoreSmspListEntryReqL(const RMobileSmsMessaging::TMobileSmspEntryV1& aSmsParam)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSmsStoreSmspListEntryComp()
 *
 * @param aSmsParam Defines a set of SMS parameters.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// Add implementation here.......

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ( ret, 
			                        KDispatchSmsFuncUnitId, 
			                        MLtsyDispatchSmsStoreSmspListEntry::KLtsyDispatchSmsStoreSmspListEntryApiId,
			                        &aSmsParam
			                       );

	return TSYLOGSETEXITERR(ret);
	} // CLtsySmsHandler::HandleStoreSmspListReqL

