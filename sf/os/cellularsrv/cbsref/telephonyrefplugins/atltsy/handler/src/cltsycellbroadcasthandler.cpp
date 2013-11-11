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
// CLtsyCellBroadcastHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>

#include "cltsycellbroadcasthandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsyCellBroadcastHandler::CLtsyCellBroadcastHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyCellBroadcastHandler::~CLtsyCellBroadcastHandler()
	{
	}


CLtsyCellBroadcastHandler* CLtsyCellBroadcastHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCellBroadcastHandler* self = new (ELeave) CLtsyCellBroadcastHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyCellBroadcastHandler* CLtsyCellBroadcastHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyCellBroadcastHandler* self=
			CLtsyCellBroadcastHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyCellBroadcastHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsyCellBroadcastHandler::ConstructL

TBool CLtsyCellBroadcastHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
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

void CLtsyCellBroadcastHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
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


TInt CLtsyCellBroadcastHandler::HandleSetBroadcastFilterSettingReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter /*aFilterSetting*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCellBroadcastSetBroadcastFilterSettingComp
 *
 * @param aFilterSetting Broadcast filter setting
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCellBroadcastHandler::HandleSetBroadcastFilterSettingReqL


TInt CLtsyCellBroadcastHandler::HandleActivateBroadcastReceiveMessageReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter /*aFilterSetting*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCellBroadcastActivateBroadcastReceiveMessageComp
 *
 * @param aFilterSetting Broadcast filter setting
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCellBroadcastHandler::HandleActivateBroadcastReceiveMessageReqL


TInt CLtsyCellBroadcastHandler::HandleReceiveMessageCancelReqL(RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter /*aFilterSetting*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCellBroadcastReceiveMessageCancelComp
 *
 * @param aFilterSetting Broadcast filter setting
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCellBroadcastHandler::HandleReceiveMessageCancelReqL
	
TInt CLtsyCellBroadcastHandler::HandleStartSimCbTopicBrowsingReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCellBroadcastStartSimCbTopicBrowsingComp
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCellBroadcastHandler::HandleStartSimCbTopicBrowsingReqL
	
TInt CLtsyCellBroadcastHandler::HandleDeleteSimCbTopicReqL(TUint /*aIndex*/, TBool /*aDeleteFlag*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackCellBroadcastDeleteSimCbTopicComp
 *
 * @param aIndex SimCb topic number.
 * @param aDeleteFlag topic in sim memory delete flag.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyCellBroadcastHandler::HandleDeleteSimCbTopicReqL
