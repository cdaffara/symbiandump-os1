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
// CLtsyPhonebookOnHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsyphonebookonhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsyPhonebookOnHandler::CLtsyPhonebookOnHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsyPhonebookOnHandler::~CLtsyPhonebookOnHandler()
	{
	}


CLtsyPhonebookOnHandler* CLtsyPhonebookOnHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookOnHandler* self = new (ELeave) CLtsyPhonebookOnHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhonebookOnHandler* CLtsyPhonebookOnHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookOnHandler* self=
			CLtsyPhonebookOnHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsyPhonebookOnHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsyPhonebookOnHandler::ConstructL

TBool CLtsyPhonebookOnHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
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

void CLtsyPhonebookOnHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
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


TInt CLtsyPhonebookOnHandler::HandleStoreReadReqL(TInt /*aIndex*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadComp
 *
 * @param aIndex index of the entry.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreReadReqL


TInt CLtsyPhonebookOnHandler::HandleStoreDeleteAllReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteAllComp
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreDeleteAllReqL


TInt CLtsyPhonebookOnHandler::HandleStoreReadEntryReqL(TInt /*aIndex*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreReadEntryComp
 *
 * @param aIndex index of the ON entry.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;



	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreReadEntryReqL


TInt CLtsyPhonebookOnHandler::HandleStoreGetInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetInfoComp
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreGetInfoReqL


TInt CLtsyPhonebookOnHandler::HandleStoreGetReadStoreSizeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetReadStoreSizeComp
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreGetReadStoreSizeReqL


TInt CLtsyPhonebookOnHandler::HandleStoreDeleteEntryReqL(TInt /*aIndex*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreDeleteEntryComp
 *
 * @param aIndex index of the ON entry.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreDeleteEntryReqL


TInt CLtsyPhonebookOnHandler::HandleStoreWriteEntryReqL(const RMobileONStore::TMobileONEntryV1& /*aMobileOnEntry*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteEntryComp
 *
 * @param aMobileOnEntry defines contents of a fixed-size ON entry to be stored.  
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;



	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreWriteEntryReqL


TInt CLtsyPhonebookOnHandler::HandleStoreWriteReqL(const RMobileONStore::TMobileONEntryV1& /*aMobileOnEntry*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreWriteComp
 *
 * @param aMobileOnEntry defines contents of a fixed-size ON entry to be stored.  
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;



	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreWriteReqL


TInt CLtsyPhonebookOnHandler::HandleStoreGetStoreSizeReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackPhonebookOnStoreGetStoreSizeComp
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;


	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookOnHandler::HandleStoreGetStoreSizeReqL





