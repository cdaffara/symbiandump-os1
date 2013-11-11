// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


#include "mSLOGGER.H"
#include "Mphbkcom.h"
#include "ATIO.H"
#include "ATINIT.H"
#include "Matstd.h"
#include "monstore.h"


	 
CMobileONStore* CMobileONStore::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aMMStatus)
/** NewL
 *
 * This method creats a instance of the CMobileONStore class. 
 * It is implented as a standard 2 phase constructor.
 */
	{
	CMobileONStore* oNStore=new(ELeave) CMobileONStore(aATIO,aInit,aMMStatus);
	CleanupStack::PushL(oNStore);
	oNStore->ConstructL();
	CleanupStack::Pop();
	return oNStore;
	}


CMobileONStore::CMobileONStore(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aMMGlobals)
	: iIo(aATIO), iInit(aInit), iPhoneGlobals(aMMGlobals)
	{}


void CMobileONStore::ConstructL()
/**
 * Construction of global params.
 */
	{
	iATOwnNumbers = CATOwnNumbers::NewL(iIo,this,iInit,iPhoneGlobals);
	}


CMobileONStore::~CMobileONStore()
/** 
 * Destructor for CMobileONStore globals.
 */
	{
	delete iATOwnNumbers;
	}


CTelObject::TReqMode CMobileONStore::ReqModeL(const TInt aIpc)
/** ReqModeL
 *
 * This function is basically a switch statement which describes the flow control
 * capabilities for each request dealt with by the CMobileONStore sub-session extension.
 * @param  aIpc
 * @return 
 */
	{
	CTelObject::TReqMode ret(0);

	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
	case EMobilePhoneStoreRead:
		ret=KReqModeFlowControlObeyed;
		break;

	default:
		User::Leave(KErrNotSupported);
		}
	
	// Check if the data port is currently loaned. If it is and the requested IPC
	// is flow controlled then block Etel calling the IPC by leaving with KErrInUse
	if((ret&KReqModeFlowControlObeyed) && iPhoneGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

	return ret;	
	}


TInt CMobileONStore::ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc,const TDataPackage& aPackage)
/** ExtFunc
 *
 * 
 * @param aTsyReqHandle the request ID
 * @param aIpc which request to execute
 * @param aPackage refence to the argument to pas in to the method specified by the IPC number. 
 * @return 
 */
	{

	switch (aIpc)
		{

	case EMobilePhoneStoreGetInfo:
		return GetOwnNumberInfo(aTsyReqHandle, aPackage.Des1n());
	case EMobilePhoneStoreRead:
		return Read(aTsyReqHandle, aPackage.Des1n());
	default:
		{
		LOGTEXT2(_L8("CPhoneMobile::ExtFunc\tunsupported request %d"), aIpc);
		ReqCompleted(aTsyReqHandle, KErrNotSupported);
		return KErrNone;
		}

		}			
	}


TInt CMobileONStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 *
 * Since this class does not support any kind off notification
 * 0 is returned as the size of the buffer needed.
 */
	{
	return 0;
	}


TInt CMobileONStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * This function is not supported.
 */
	{
	return KErrNotSupported;
	}


TInt CMobileONStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * This function is not supported.
 */
	{
	return KErrNotSupported;
	}


void CMobileONStore::Init()
/**
 * This function is not supported.
 */
	{}


TInt CMobileONStore::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
/** Cancel Service
 * @param aIpc which method to cancel
 * @param aTsyReqHandle the request ID to the request to cancel
 * @return 
 */
	{
	switch(aIpc)
		{
		case EMobilePhoneStoreRead:
			return ReadCancel(aTsyReqHandle);
		default:
			return KErrNotSupported;
		}//switch
	}


CTelObject* CMobileONStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * This function is not supported.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject* CMobileONStore::OpenNewObjectL(TDes& /*aName*/)
/**
 * This function is not supported.
 */
	{	
	User::Leave(KErrNotSupported);
	return NULL;
	}


TInt CMobileONStore::GetOwnNumberInfo(TTsyReqHandle aTsyReqHandle, TDes8* aInfo)
/** Get Own Number Info
 * 
 * This method pases on a request to get information about the own number store. 
 * The information retrieved is containing 
 * @param aTsyReqHandle the request ID
 * @param aInfo Own number 
 * @return Error code
 */
	{
	iATOwnNumbers->GetInfo(aTsyReqHandle,aInfo);
	return KErrNone;
	}


TInt CMobileONStore::Read(TTsyReqHandle aTsyReqHandle, TDes8* aEntry)
/** Read a Own Number Store entry
 *
 * This method deals with the request to get one entry from the own number store. 
 * The one entry to get is specified by the iIndex member of the TMobileONEntryV1 
 * class.
 * The iIndex is part of aEntry and is used in CATOwnNumbers::GetEntry to get the 
 * entry from the phone. The request is passed on to the CATOwnNumbers class which 
 * executes the AT+CNUM command(if needed) and returns the dersired information. 
 * @param aTsyReqHandle The request ID
 * @param aEntry This argument is of type TMobileONEntryV1Pckg.
 * @return Error code
 */
	{
	//Pass the request on to the CATOwnNumbers class.
	iATOwnNumbers->GetEntry(aTsyReqHandle, aEntry);
	return KErrNone; 
	}


TInt CMobileONStore::ReadCancel(const TTsyReqHandle aTsyReqHandle)
/** Cancel Raed
 *
 * This method passae the request to cancel a read on to the CATOwnNumbers class.
 * @param aTsyReqHandle The request ID
 * @return Error code
 */
	{
	iATOwnNumbers->CancelCommand(aTsyReqHandle);
	return KErrNone;
	} 
