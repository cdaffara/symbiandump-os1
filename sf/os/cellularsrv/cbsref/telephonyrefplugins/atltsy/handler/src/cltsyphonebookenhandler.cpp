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
// CLtsyPhonebookEnHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>



#include "cltsyphonebookenhandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"
#include "globalphonemanager.h"
#include "atphbkengetinfo.h"
#include "atphbkenread.h"
#include "atphbkenreadall.h"

CLtsyPhonebookEnHandler::CLtsyPhonebookEnHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	:iCtsyDispatcherCallback(aCtsyDispatcherCallback),iGloblePhone(aGloblePhone)

	{
	}

	
CLtsyPhonebookEnHandler::~CLtsyPhonebookEnHandler()
	{
	}


CLtsyPhonebookEnHandler* CLtsyPhonebookEnHandler::NewLC(
		CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookEnHandler* self = new (ELeave) CLtsyPhonebookEnHandler(aCtsyDispatcherCallback,aGloblePhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsyPhonebookEnHandler* CLtsyPhonebookEnHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback,CGlobalPhonemanager* aGloblePhone)
	{
	TSYLOGENTRYEXIT;
	CLtsyPhonebookEnHandler* self=
			CLtsyPhonebookEnHandler::NewLC(aCtsyDispatcherCallback,aGloblePhone);
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

	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		case  KLtsyDispatchPhonebookEnStoreGetInfoApiId:
			return ETrue;
		case KLtsyDispatchPhonebookEnStoreReadAllApiId:
			return ETrue;
		case KLtsyDispatchPhonebookEnStoreReadEntryApiId:
			return ETrue;
		default:
			return EFalse;
		}
	}

void CLtsyPhonebookEnHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
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

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkEnGetInfo* atPhbkEnGetInfo = CAtPhbkEnGetInfo::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkEnGetInfo);
		atPhbkEnGetInfo->StartRequest();
		}

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

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkEnReadAll* atPhbkEnReadAll = CAtPhbkEnReadAll::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkEnReadAll);
		atPhbkEnReadAll->StartRequest();
		}

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

	TInt ret = iGloblePhone->CheckGlobalPhoneStatus();
	if( KErrNone == ret )
		{
		CAtPhbkEnRead* atPhbkEnRead = CAtPhbkEnRead::NewL(*iGloblePhone,iCtsyDispatcherCallback);
		//Setting Port active
		iGloblePhone->iEventSignalActive = ETrue;
		iGloblePhone->SetActiveRequest(atPhbkEnRead);
		atPhbkEnRead->SetIndex(aIndex);
		atPhbkEnRead->StartRequest();
	    }

	return TSYLOGSETEXITERR(ret);
	} // CLtsyPhonebookEnHandler::HandleStoreReadEntryReqL





