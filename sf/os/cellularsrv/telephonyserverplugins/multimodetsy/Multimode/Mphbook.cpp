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
// \file MPHBOOK.CPP
// Mulimode TSY Phone book access Implementation file.
// This file contains the implementation of the CMobilePhonebookStore 
// class which is used for GSM phone book access. 
// 
//

#include "mSLOGGER.H"
#include "mPHBOOK.H"
#include "Mphbkcom.h"
#include "ATINIT.H"
#include "NOTIFY.H"
#include <etelmm.h>

//
// CPhoneBook definitions
//

CMobilePhonebookStore* CMobilePhonebookStore::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals,const TDesC8& aStorageType)
/**
 * Standard 2 phase constructor.
 */
	{
	CMobilePhonebookStore* subsession=new(ELeave) CMobilePhonebookStore(aATIO,aInit,aPhoneGlobals,aStorageType);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

CMobilePhonebookStore::CMobilePhonebookStore(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aMmGlobals,const TDesC8& aStorageType)
	: iIo(aATIO), iInit(aInit), iPhoneGlobals(aMmGlobals), iStorageType(aStorageType)
	{	
	}

void CMobilePhonebookStore::ConstructL()
/**
 * ConstructL() function. Creates CATPhoneBookInit, CATPhoneBookWrite, CATPhoneBookDelete
 * objects.
 */
	{
	iInfo=CATPhoneBookInfo::NewL(iIo,this,iInit,iPhoneGlobals);
	iRead=CATPhoneBookRead::NewL(iIo,this,iInit,iPhoneGlobals,iInfo);
	iWrite=CATPhoneBookWrite::NewL(iIo,this,iInit,iPhoneGlobals);
	iDelete=CATPhoneBookDelete::NewL(iIo,this,iInit,iPhoneGlobals);

	// Assume that all phone books have at least read access
	iReadWriteAccess|=RMobilePhoneStore::KCapsReadAccess;

	// Check if phone book should have write access	
	if (CPhoneGlobals::IsWriteAccess(iStorageType))
		iReadWriteAccess |= RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsReadAccess;
	}

CMobilePhonebookStore::~CMobilePhonebookStore()
/**
 * Destructor.
 */
	{
	if (iPhoneGlobals != NULL)
		iPhoneGlobals->iNotificationStore->RemoveClientFromLastEvents(this); //Joe
	delete iRead;
	delete iWrite;
	delete iDelete;
	delete iInfo;
	}

CTelObject::TReqMode CMobilePhonebookStore::ReqModeL(const TInt aIpc)
/**
 * This function is basically a switch statement which describes the flow control
 * capabilities for each request dealt with by the CPhoneBook sub-session extension.
 */
	{
	CTelObject::TReqMode ret(0);

	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		ret=KReqModeFlowControlObeyed|KReqModeMultipleCompletionEnabled;
		break;

	case EMobilePhoneBookStoreWrite:
	case EMobilePhoneBookStoreRead:
	case EMobilePhoneStoreDelete:
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

TInt CMobilePhonebookStore::NumberOfSlotsL(const TInt /*aIpc*/)
/**
 * This function returns the number of slots (buffered in server) for any 
 * KReqRepostImmediately Ipc calls in the ReqModeL() function.
 */
	{
	return 1;
	}

TInt CMobilePhonebookStore::RegisterNotification(const TInt /*aIpc*/)
/**
 * This function is not supported.
 */
	{
	return KErrNone;
	}

TInt CMobilePhonebookStore::DeregisterNotification(const TInt /*aIpc*/)
/**
 * This function is not supported.
 */
	{
	return KErrNone;
	}

void CMobilePhonebookStore::Init()
	{
	LOGTEXT(_L8("CMobilePhonebookStore::Init() called"));
	}

TInt CMobilePhonebookStore::CancelService(const TInt aIpcToCancel,const TTsyReqHandle aTsyReqHandle)
/**
 * This function is called when an asynchronous request has passed to the TSY module
 * but is yet to be completed. It's used to cancel a client's service request.
 */
	{
	switch (aIpcToCancel)
		{
	case EMobilePhoneBookStoreWrite:
		iWrite->CancelCommand(aTsyReqHandle); 
		break;
	case EMobilePhoneBookStoreRead:
		iRead->CancelCommand(aTsyReqHandle);
		break;
	case EMobilePhoneStoreDelete:
		iDelete->CancelCommand(aTsyReqHandle);
		break; 
	case EMobilePhoneStoreGetInfo: 
		iInfo->CancelCommand(aTsyReqHandle); 
		break;
	default:	// & synchronous functions, nothing to cancel
		break;
		}
	return KErrNone;
	}

CTelObject* CMobilePhonebookStore::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * This function is not supported.
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}
	
CTelObject* CMobilePhonebookStore::OpenNewObjectL(TDes& /*aName*/)
/**
 * This function is not supported.
 */
	{	
	User::Leave(KErrNotSupported);
	return NULL;
	}
	

TInt CMobilePhonebookStore::ExtFunc(const TTsyReqHandle aTsyReqHandle, const TInt aIpc,const TDataPackage& aPackage)
/**
 * This function is responsible for dealing with IPC requests the server doesn't support.
 */
	{
	// Can't initialise if the port is tied up...
	if((iPhoneGlobals->iPhoneStatus.iPortAccess==EPortAccessDenied) || (iPhoneGlobals->iPhoneStatus.iMode == RPhone::EModeOnlineData))
		{
		LOGTEXT2(_L8("CMobilePhonebookStore::ExtFunc (aIpc=%d)"),aIpc);
		LOGTEXT(_L8("CMobilePhonebookStore::ExtFunc\tPort Access Denied/Mode Online flag detected, returning KErrAccessDenied error"));

		return KErrAccessDenied;
		}

	// we can handle the request now
	return HandleExtFunc(aTsyReqHandle,aIpc,aPackage);
	}

TInt CMobilePhonebookStore::HandleExtFunc(TTsyReqHandle aTsyReqHandle,TInt aIpc,const TDataPackage& aPackage)
/**
 * Handle the requested function. The phone book data is now fully live.
 */
	{
	// We're in a state to execute commands...
	TAny* dataPtr1=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();

	switch (aIpc)
		{
	case EMobilePhoneStoreGetInfo:
		return GetInfo(aTsyReqHandle, aPackage.Des1n());

	case EMobilePhoneBookStoreRead:
		return Read(aTsyReqHandle, reinterpret_cast<RMobilePhoneBookStore::TPBIndexAndNumEntries*>(dataPtr1), aPackage.Des2n());

	case EMobilePhoneBookStoreWrite:
		return Write(aTsyReqHandle, aPackage.Des1n(), reinterpret_cast<TInt*>(dataPtr2));

	case EMobilePhoneStoreDelete:
		return Delete(aTsyReqHandle,reinterpret_cast<TInt*>(dataPtr1));	
	
	default:
		return KErrNotSupported;
		}
	}

TInt CMobilePhonebookStore::GetInfo(TTsyReqHandle aTsyReqHandle, TDes8* aInfoPckg)
/** Get Phone Book Storage Information
 *
 * This function queries the phone for storage information on the current phone book
 * store. 
 * It returns the related storage information only if supported by the phone.
 * @param aTsyReqHandle Handle to the request ID .
 * @param aInfoPckg Pointer to the struct that contains information about the phone book.
 * @return error code. 
 */
	{
	LOGTEXT(_L8("CMobilePhonebookStore::GetInfo called"));

	// Have to send AT+CPBR=? to get the phonebook information
	// This command is sent every time to ensure we have up-to-date knowledge
	// of used number of entries
	iInfo->SetStorageType(iStorageType);
	iInfo->ExecuteCommand(aTsyReqHandle, aInfoPckg);
	
	return KErrNone;
	}

TInt CMobilePhonebookStore::Read(TTsyReqHandle aTsyReqHandle, RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries, TDes8* aPBData)
/** Read one or a number of Phone Book Enties
 *
 * This function reads the phone book entries from the current
 * phone book store.
 * @param aTsyReqHandle Handle to the request ID.
 * @param aIndexAndEntries is a pointer to an index and a number of entries.
 * @param aPBData is a pointer to a packet.
 * @return error code. 
 */
	{
  LOGTEXT(_L8("CMobilePhonebookStore::Read called"));

	if (!(iReadWriteAccess&RMobilePhoneStore::KCapsReadAccess)) 
		{ 
	  LOGTEXT(_L8("CMobilePhonebookStore::Read Completing with KErrAccessDenied as read access is not allowed"));
		ReqCompleted(aTsyReqHandle,KErrAccessDenied); 
		return KErrNone;  
		}	

	iRead->SetPBData(aPBData);

	// Store index & number of entries so we can modify/truncate it if required
	iIndexAndEntries=*aIndexAndEntries;

	// If we already have information about this phonebook
	// Check client has supplied valid locations - otherwise send command anyway
	if (iInfo->Completed())
		{
		// Allow CATPhoneBookInfo to map our clients index value (1..x)
		// the this specific phone store index values (1+y..x+y)
		// For the Motorola T260 SM phone book store y will be 100
		LOGTEXT(_L8("CMobilePhonebookStore::Read Mapping clients index to phone index"));
		iInfo->MapClientIndexToPhoneIndex(iIndexAndEntries.iIndex);

		if (iInfo->IsValidPhoneIndex(iIndexAndEntries.iIndex))
			{
			if (!iInfo->IsValidPhoneIndex(iIndexAndEntries.iIndex+iIndexAndEntries.iNumSlots-1)) 
				{
				// Invalid finishing index - truncate to last valid one
				iIndexAndEntries.iNumSlots = iInfo->TotalEntries()-iIndexAndEntries.iIndex+1 ; 
				}
			}
		else
			{
			// Invalid starting index - complete request with error
			LOGTEXT(_L8("CMobilePhonebookStore::Read\t Index out of range"));
			ReqCompleted(aTsyReqHandle,KErrArgument); // The Index is out of range.
			return KErrNone;
			}
		}

	// We either have valid index or entries - or are trying anyway
	iRead->ExecuteCommand(aTsyReqHandle, &iIndexAndEntries);   
	return KErrNone;
	}


TInt CMobilePhonebookStore::Write(TTsyReqHandle aTsyReqHandle, TDes8* aPBData, TInt* aIndex)
/**  Write a Phone Book Entry
 *
 * This function checks to see if the current phone book has Write Access; if it
 * does, an entry is written to the phone book store.
 * The function first creates the entry, then ensures there is space in the data 
 * object, before writing the entry. 
 * @param aTsyReqHandle Handle to the request ID.
 * @param aPBData is a pointer to a packet.
 * @param aIndex is a pointer to an entry in the phone book. 
 * @return error code. 
 */
	{
	LOGTEXT(_L8("CMobilePhonebookStore::Write called"));
	
	if(!(iReadWriteAccess&RMobilePhoneStore::KCapsWriteAccess))
		{
		ReqCompleted(aTsyReqHandle,KErrAccessDenied);
		return KErrNone;
		}

	// Take copy of clients data, as we do not want changes that we make here
	// to be reflected in the clients space.
	iIndex=*aIndex;

	iWrite->SetPBData(aPBData);

	// If we already have information about this phonebook
	// Check client has supplied valid location - otherwise send command anyway
	if (iInfo->Completed()) 
		{
		// Allow CATPhoneBookInfo to map our clients index value (1..x)
		// the this specific phone store index values (1+y..x+y)
		// For the Motorola T260 SM phone book store y will be 100
		LOGTEXT(_L8("CMobilePhonebookStore::Write Mapping clients index to phone index"));
		iInfo->MapClientIndexToPhoneIndex(iIndex);
	
		if(!iInfo->IsValidPhoneIndex(iIndex))
			{
			LOGTEXT(_L8("CMobilePhonebookStore::Write\t Index out of range"));
			ReqCompleted(aTsyReqHandle,KErrArgument); // The Index is out of range.
			return KErrNone;
			}
		}

	iWrite->ExecuteCommand(aTsyReqHandle, &iIndex); 
	return KErrNone;
	}


TInt CMobilePhonebookStore::Delete(TTsyReqHandle aTsyReqHandle,TInt* aIndex)
/** Delete a Phonebook Entry
 *
 * This function deletes a phonebook entry.
 * The data object is only updated when the AT command completes successfully.
 * @param aTsyReqHandle Handle to the request ID.
 * @param aIndex is a pointer to an entry in the phone book. 
 * @return error code. 
 */
	{ 
	LOGTEXT(_L8("CMobilePhonebookStore::Delete called"));	
	if(!(iReadWriteAccess & RMobilePhoneStore::KCapsWriteAccess)) 
		{	
		ReqCompleted(aTsyReqHandle,KErrAccessDenied); 
		return KErrNone; 	
		}

	// Take copy of clients data, as we do not want changes that we make here
	// to be relected in the clients space.
	iIndex=*aIndex;

	// If we already have information about this phonebook
	// Check client has supplied valid location - otherwise send command anyway
	if (iInfo->Completed()) 
		{
		// Allow CATPhoneBookInfo to map our clients index value (1..x)
		// the this specific phone store index values (1+y..x+y)
		// For the Motorola T260 SM phone book store y will be 100
		LOGTEXT(_L8("CMobilePhonebookStore::Delete Mapping clients index to phone index"));
		iInfo->MapClientIndexToPhoneIndex(iIndex);

		if(!iInfo->IsValidPhoneIndex(iIndex))
			{
			LOGTEXT(_L8("CMobilePhonebookStore::Delete\t Index out of range"));
			ReqCompleted(aTsyReqHandle,KErrArgument); // The Index is out of range.
			return KErrNone;
			}
		}

	iDelete->ExecuteCommand(aTsyReqHandle,&iIndex);
	return KErrNone;
	}


