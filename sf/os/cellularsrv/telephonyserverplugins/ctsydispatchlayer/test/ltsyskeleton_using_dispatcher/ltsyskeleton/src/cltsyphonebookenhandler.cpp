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
// cltsydispatchphonebookenhandler.cpp
//

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>



#include "cltsyphonebookenhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"

// Can be removed in real LTSY implementation
#include MTEST_INCLUDE_MOCKLTSYENGINE_HEADER

CLtsyPhonebookEnHandler::CLtsyPhonebookEnHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyPhonebookEnHandler::~CLtsyPhonebookEnHandler()
	{
	}


CLtsyPhonebookEnHandler* CLtsyPhonebookEnHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookEnHandler* self = new (ELeave) CLtsyPhonebookEnHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhonebookEnHandler* CLtsyPhonebookEnHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookEnHandler* self=
			CLtsyPhonebookEnHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPhonebookEnHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsyPhonebookEnHandler::ConstructL

TBool CLtsyPhonebookEnHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(KDispatchPhonebookEnFuncUnitId, aDispatchApiId);
	
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

void CLtsyPhonebookEnHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask)
/**
 * Returns the set of callback indicators that are supported.
 * 
* @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
* @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{
	// REMOVE this line when implementing for real LTSY
	MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(KDispatchPhonebookEnFuncUnitId, aIdGroup, aIndIdBitMask);
	
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


TInt CLtsyPhonebookEnHandler::HandleStoreGetInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreGetInfoComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhonebookEnFuncUnitId, MLtsyDispatchPhonebookEnStoreGetInfo::KLtsyDispatchPhonebookEnStoreGetInfoApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookEnHandler::HandleStoreGetInfoReqL


TInt CLtsyPhonebookEnHandler::HandleStoreReadAllReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadAllComp()
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ3(ret, KDispatchPhonebookEnFuncUnitId, MLtsyDispatchPhonebookEnStoreReadAll::KLtsyDispatchPhonebookEnStoreReadAllApiId);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookEnHandler::HandleStoreReadAllReqL


TInt CLtsyPhonebookEnHandler::HandleStoreReadEntryReqL(TInt aIndex)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookEnStoreReadEntryComp()
 *
 * @param aIndex index of the EN entry.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// REMOVE this line when implementing for real LTSY
	MTEST_ISSUE_MOCKLTSYENGINE_REQ(ret, 
								   KDispatchPhonebookEnFuncUnitId, 
								   MLtsyDispatchPhonebookEnStoreReadEntry::KLtsyDispatchPhonebookEnStoreReadEntryApiId,
								   aIndex);

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookEnHandler::HandleStoreReadEntryReqL





