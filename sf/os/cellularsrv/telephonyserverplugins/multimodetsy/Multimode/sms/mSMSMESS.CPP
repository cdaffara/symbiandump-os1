// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements Sms Extensions
// 
//

#include <etelmm.h>
#include <etelmmerr.h>		// for Multimode specific error codes

#include "ATINIT.H"
#include "mSMSMESS.H"
#include "mSLOGGER.H"
#include "mSMSSEND.H"
#include "mSMSCSCA.H"
#include "mSSETSC.H"
#include "set_cmti_mode.h"		// for CATSetPhoneToCMTIMode
#include "set_cmt_mode.h"		// for CATSetPhoneToCMTMode
#include "sms_rx_queue.h"       // for CReceiveSmsQueue
#include "cmti_stored.h"		// for CATSmsWaitForAndHandleCMTIStored
#include "cmt_unstored.h"		// for CATSmsWaitForAndHandleCMTUnstored
#include "panic.h"			    // TSY panic codes and ::Panic function


//
// Macros
//
#define UNPACK_PCKG(target,source,datatype) datatype& target=(*(TPckg<datatype>*)source)()

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CMobileSmsMessaging::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


//
// Class Implementation
// 
CMobileSmsMessaging* CMobileSmsMessaging::NewL(CATIO* aATIO,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
/**
 * Standard NewL constructor.
 */
	{
	CMobileSmsMessaging* subsession=new(ELeave) CMobileSmsMessaging(aATIO,aInit,aPhoneGlobals);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}


CMobileSmsMessaging::CMobileSmsMessaging(CATIO* aIo,CATInit* aInit,CPhoneGlobals* aGlobals)
	:iIo(aIo)
	,iInit(aInit)
	,iGlobals(aGlobals)
	,iCurrentRxMode(RMobileSmsMessaging::EReceiveModeUnspecified)
/**
 * C++ constructor
 */
	{
	}

void CMobileSmsMessaging::ConstructL()
/**
 * Standard 2nd phase constructor.
 * Creates the AT objects which are used to communicate with phone.
 */
	{
	iATSmsMessagingSend = CATSmsMessagingSend::NewL(iIo,this,iInit,iGlobals);
	iATSmsMemoryStorage = CATSmsMemoryStorage::NewL(iIo,this,iInit,iGlobals);
	iATSmsGetSCAddress = CATSmsGetSCAddress::NewL(iIo,this,iInit,iGlobals);
	iATSmsSetSCAddress = CATSmsSetSCAddress::NewL(iIo,this,iInit,iGlobals);
	iGetSmspList = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1); 

	// Create the object which we will use to set the phone to use CMTI mode
	iATSetPhoneToCMTIMode=CATSetPhoneToCMTIMode::NewL(iIo,this,iInit,iGlobals,iCurrentRxMode);

	// Create the object which we will use to set the phone to use CMT mode
	iATSetPhoneToCMTMode=CATSetPhoneToCMTMode::NewL(iIo,this,iInit,iGlobals,iCurrentRxMode);

	// Create queue for received SMS messages
	iReceiveSmsQueue=CReceiveSmsQueue::NewL(iIo,this,iInit,iGlobals,*this);

	// Store pointer to our CReceiveSmsQueue object in CPhoneGlobals
	// so that the CATCommands::Complete method can prod the object when 
	// the serial port is not being used. This is to allow the CRececiveSmsQueu
	// object to read any PDUs from the phones memory that it needs to.
	// Ownership of the iReceiveSmsQueue stays with this class.
	iGlobals->iReceiveSmsQueuePtr=iReceiveSmsQueue;

	// Create the object that will listen for CMTI commands from the phone
	// and will populate our received SMS queue
	iWaitForCMTIStored=CATSmsWaitForAndHandleCMTIStored::NewL(iIo,this,iGlobals,*iReceiveSmsQueue);

	// Create the object that will listen for CMT commands from the phone
	// and will populate our received SMS queue
	iWaitForCMTUnstored=CATSmsWaitForAndHandleCMTUnstored::NewL(iIo,this,iGlobals,*iReceiveSmsQueue);
	}

CMobileSmsMessaging::~CMobileSmsMessaging()
/**
 * C++ destructor
 * Destroys the AT objects created during construction
 */
	{
	delete iWaitForCMTUnstored;
	delete iWaitForCMTIStored;

	// We gave CPhoneGlobals a copy of the pointer to iReceiveSmsQueue.
	// So we have to null that copy before we destory the one and only
	// CReceiveSmsQueue object which this class owns.
	if(iGlobals)
		iGlobals->iReceiveSmsQueuePtr=NULL;
	delete iReceiveSmsQueue;

	delete iATSetPhoneToCMTMode;
	delete iATSetPhoneToCMTIMode;

	if (iGlobals)
		iGlobals->iNotificationStore->RemoveClientFromLastEvents(this);
	delete iATSmsMessagingSend;
	delete iATSmsMemoryStorage;
	delete iATSmsGetSCAddress;
	delete iATSmsSetSCAddress;
	if(iGetSmspList)		// This prevents an Access Violation when CommDB is missing
		{
		iGetSmspList->ResetAndDestroy();	
		delete iGetSmspList;
		}
	}


TInt CMobileSmsMessaging::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Attempts to cancel a service.
 * @param aIpc The number of the IPC to be cancelled
 * @param aTsyReHandle The handle of the request which started the IPC
 * @return Standard KErr... codes
 */	{
	TInt ret(KErrNone);
	switch (aIpc)
		{
	case EMobileSmsMessagingGetMessageStoreInfo:
		iATSmsMemoryStorage->CancelCommand(aTsyReqHandle);
		break;
			
	case EMobileSmsMessagingSendMessage:
		iATSmsMessagingSend->CancelCommand(aTsyReqHandle);
		break;

	case EMobileSmsMessagingReceiveMessage:
		CancelReceiveMessage();
		break;

	case EMobileSmsMessagingGetSmspListPhase1:
		iATSmsGetSCAddress->CancelCommand(aTsyReqHandle);
		break;

	case EMobileSmsMessagingStoreSmspList:
		iATSmsSetSCAddress->CancelCommand(aTsyReqHandle);
		break;

	case EMobileSmsMessagingSetReceiveMode:
	case EMobileSmsMessagingGetSmspListPhase2:
	default:
		// can't do anything to cancel these requests
		break;

		}
	return ret;	
	}

void CMobileSmsMessaging::Init()
/**
 * TSY Intialization
 * This method has to be implemented as it is pure virtual in MTelObjectTSY.
 * In this case there is no intialization that needs to be done.
 */
	{}


CTelObject* CMobileSmsMessaging::OpenNewObjectByNameL(const TDesC& aName)
/**
 * This method responds to request made by a client of Etel to create a new object of some kind.
 * Known names are defined in ETELMM.H with the following identifiers...
 *   KETelMeSmsStore 
 *   KETelCombinedSmsStore
 *   KETelIccSmsStore
 * @param aName Descriptor holding the name of the new object to be created
 * @return A pointer to the newly created object or NULL if aName was not recognised
 */
	{
	if (aName.Compare(KETelMeSmsStore)==0)
		{
		TStorageType storeType(KMEStorage);
		return CMobileSmsStore::NewL(iIo,iInit,iGlobals,storeType);
		}

	if (aName.Compare(KETelIccSmsStore)==0)
		{
		TStorageType storeType(KSMStorage);
		return CMobileSmsStore::NewL(iIo,iInit,iGlobals,storeType);
		}

	if (aName.Compare(KETelCombinedSmsStore)==0)
		{
		TStorageType storeType(KMTStorage);
		return CMobileSmsStore::NewL(iIo,iInit,iGlobals,storeType);
		}

	// Store not supported
	User::Leave(KErrNotFound);
	return NULL;
	}


CTelObject* CMobileSmsMessaging::OpenNewObjectL(TDes& /*aNewName*/)
/**
 * The use of this method is not supported in the MMTSY.
 * @leave This method always leaves with KErrNotSupported
 */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CTelObject::TReqMode CMobileSmsMessaging::ReqModeL(const TInt aIpc)
/**
 * This method returns the KReq... mode(s) for a given IPC.
 * @leave This method will leave with KErrNotSupported if aIpc is not recognized.
 * @param aIpc The number of the IPC which is being queried
 * @return The request mode to use for aIpc
 */
	{
	// Check the IPC number is valid
	if(!IsSupportedIPC(aIpc))
		User::Leave(KErrNotSupported);
	
	CTelObject::TReqMode ret=0;	
	switch (aIpc)
		{
		// TSYs wishing to implement their own buffering algorithm will place all requests in this category.  
		// For SMS - ReceiveMessage is in this category because incoming SMS are queued in TSY
	case EMobileSmsMessagingReceiveMessage:
	case EMobileSmsMessagingGetReceiveMode:
	case EMobileSmsMessagingGetCaps:		
	case EMobileSmsMessagingGetSmspListPhase2:
		ret=0;
		break;

		// KReqModeFlowControlObeyed
		// Commands that change the state of the modem, e.g. setting the monitor volume level; in other words,
		// those commands that the TSY should only deal with one at a time.
	case EMobileSmsMessagingSetReceiveMode:		
	case EMobileSmsMessagingSendMessage:
	case EMobileSmsMessagingEnumerateMessageStores:
	case EMobileSmsMessagingGetMessageStoreInfo:
	case EMobileSmsMessagingGetSmspListPhase1:
	case EMobileSmsMessagingStoreSmspList:
		ret=KReqModeFlowControlObeyed;
		break;

		// KReqModeRePostImmediately
		// Requests that notify a client about a change of state, where the TSY needs to distinguish between
		// different clients.


		// KReqModeMultipleCompletionEnabled
		// Either (a) commands that may take some time, but which the TSY can handle more than one of concurrently, or 
		// (b) notifications that the TSY does not wish to be re-posted immediately, so the server does no buffering.

		// KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
		// Commands that request information from the modem that is not cached, hence takes a finite amount of time.
		// The TSY should only deal with one of these commands at a time.

		// KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately
		// Requests that notify a client about a change of state.  Since these requests do not require the issuing 
		// of any modem commands, they do not have to obey flow control. The TSY never gets more than one of these
		// outstanding per TelObject.
	case EMobileSmsMessagingNotifySmspListChange:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
		}

	// Check if the data port is currently loaned. If it is and the requested IPC
	// is flow controlled then block Etel calling the IPC by leaving with KErrInUse
	if((ret&KReqModeFlowControlObeyed) && iGlobals->iPhoneStatus.iDataPortLoaned)
		{
		LOGTEXT2(_L8("ReqModeL Leaving with KErrInUse as data port is loaned (aIpc=%d)"),aIpc);
		User::Leave(KErrInUse);
		}

	return ret;
	}

TInt CMobileSmsMessaging::NumberOfSlotsL(const TInt /*aIpc*/)
	{
	// No SMS notifications are supported by this TSY
	User::Leave(KErrNotSupported);
	return 1;		// Execution should never get here
	}

TInt CMobileSmsMessaging::RegisterNotification(const TInt /*aIpc*/)
	{
	// No SMS notifications are supported by this TSY
	return KErrNone;
	}

TInt CMobileSmsMessaging::DeregisterNotification(const TInt /*aIpc*/)
	{
	// No SMS notifications are supported by this TSY
	return KErrNone;
	}

TInt CMobileSmsMessaging::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
 * Sms Read/Send Dispatch Function.
 *
 * If this method returns with an KErr code apart from KErrNone then Etel will 
 * complete and destory the clients request for us. 
 *
 * @param aTsyReqHandle The handle of the request which started the IPC
 * @param aIpc The IPC being requested
 * @param aPackage Package of parameters associated with the IPC
 * @return Standard KErr... codes
*/
	{
	if(!IsSupportedIPC(aIpc))
		return KErrNotSupported;
	
	// Prior to dispatch check that we're not setting up or in the middle of a data or fax call
	if((iGlobals->iPhoneStatus.iPortAccess==EPortAccessDenied) || (iGlobals->iPhoneStatus.iMode == RPhone::EModeOnlineData))
		{
		LOGTEXT2(_L8("CMobileSmsMessaging::ExtFunc (aIpc=%d)"),aIpc);		
		LOCAL_LOGTEXT("ExtFunc","Port Access Denied/Mode Online flag detected");
		switch(aIpc)
			{
		// These may interfere with Fax or Data call, so error the request now...
		case EMobileSmsMessagingSendMessage:
		case EMobileSmsMessagingSetReceiveMode:
		case EMobileSmsMessagingStoreSmspList:
		case EMobileSmsMessagingEnumerateMessageStores:
		case EMobileSmsMessagingGetSmspListPhase1:
		case EMobileSmsMessagingGetMessageStoreInfo:
			LOCAL_LOGTEXT("ExtFunc","Returning KErrAccessDenied as request may intefere with fax");
			return KErrAccessDenied;
			}
		}

	//
	// Handle IPC which use one parameter
	__ASSERT_DEBUG(aPackage.Des1n(),Panic(EMobileSmsMessagingNullParameter));
	switch(aIpc)
		{
	case EMobileSmsMessagingSetReceiveMode:
		return SetReceiveMode(aTsyReqHandle,aPackage.Des1n());
	
	case EMobileSmsMessagingGetReceiveMode:
		return GetReceiveMode(aTsyReqHandle,aPackage.Des1n());
	
	case EMobileSmsMessagingGetCaps:
		return GetCaps(aTsyReqHandle,aPackage.Des1n());

	case EMobileSmsMessagingStoreSmspList:
		return StoreSmspList(aTsyReqHandle,aPackage.Des1n());

	case EMobileSmsMessagingEnumerateMessageStores:
		return EnumerateMessageStores(aTsyReqHandle, aPackage.Des1n());
		}

	//
	// Handle IPC which use two parameters
	__ASSERT_DEBUG(aPackage.Des2n(),Panic(EMobileSmsMessagingNullParameter));

	switch (aIpc)
		{
	case EMobileSmsMessagingGetSmspListPhase1:
		return GetSmspListPhase1(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());

	case EMobileSmsMessagingGetSmspListPhase2:
		return GetSmspListPhase2(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());

	case EMobileSmsMessagingSendMessage:	
		return SendMessage(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());

	case EMobileSmsMessagingReceiveMessage:
		return ReceiveMessage(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());

	case EMobileSmsMessagingGetMessageStoreInfo:
		return GetMessageStoreInfo(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());
		}

	// Never expect code to execute this far
	__ASSERT_DEBUG(EFalse,Panic(EMobileSmsMessagingUnexpectedState));
	return KErrNotSupported;		
	}


//
// Methods called by ExtFunc...
//
TInt CMobileSmsMessaging::SetReceiveMode(const TTsyReqHandle aTsyReqHandle,TDes8* aParam1)
/**
 * Called by ExtFunc. 
 */
	{
	// Unpack parameters to lowest usable data type and then process them
	UNPACK_PCKG(mode,aParam1,RMobileSmsMessaging::TMobileSmsReceiveMode);
	TInt ret(KErrNone);

	switch(mode)
		{
	case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:
		LOCAL_LOGTEXT("SetReceiveMode","Client has requested EReceiveUnstoredPhoneAck mode");
		iATSetPhoneToCMTMode->ExecuteCommand(aTsyReqHandle,NULL);
		break;
			
	case RMobileSmsMessaging::EReceiveStored:
		LOCAL_LOGTEXT("SetReceiveMode","Client has requested EReceiveStored mode");
		iATSetPhoneToCMTIMode->ExecuteCommand(aTsyReqHandle,NULL);
		break;
	
	default:
		ret=KErrNotSupported;
		}

	return ret;
	}

TInt CMobileSmsMessaging::GetReceiveMode(const TTsyReqHandle aTsyReqHandle,TDes8* aParam1)
/**
 * Return the current SMS Reception Mode.
 */
	{
	LOCAL_LOGTEXT("GetReceiveMode","Enter function");

	// Unpack parameters to lowest usable data type and then process them
	UNPACK_PCKG(recMode,aParam1,RMobileSmsMessaging::TMobileSmsReceiveMode);
	
	//
	// Pass current rx mode to client
	recMode=iCurrentRxMode;
	
	//
	// Complete clients request
	ReqCompleted(aTsyReqHandle,KErrNone);

	LOCAL_LOGTEXT("GetReceiveMode","Exit function");
	return KErrNone;
	}

TInt CMobileSmsMessaging::ReceiveMessage(const TTsyReqHandle aReqHandle,TDes8* aParam1,TDes8* aParam2)
/**
 * Called by ExtFunc to process a client request to receive an incoming SMS message.
 */
	{
	LOCAL_LOGTEXT("ReceiveMessage","Enter function");

	if (iCurrentRxMode == RMobileSmsMessaging::EReceiveModeUnspecified)
		{
		return(KErrNotSupported);
		}

	// Validate that phone has been put into a known receive mode
	__ASSERT_DEBUG(iCurrentRxMode==RMobileSmsMessaging::EReceiveStored || 
			       iCurrentRxMode==RMobileSmsMessaging::EReceiveUnstoredPhoneAck,Panic(EMobileSmsMessagingPhoneNotSetToASupportedReceiveMode));

	if (iReceiveSmsQueue->ClientReqOutstanding())
		{
		// Client request already posted
		// TSY only supports one client at a time waiting for an SMS
		return KErrInUse;
		}

	// Ensure our CMT or CMTI listener has been enabled (as appropriate)
	if (iCurrentRxMode == RMobileSmsMessaging::EReceiveStored)
		{
		iWaitForCMTIStored->Enable();
		}
	else if (iCurrentRxMode == RMobileSmsMessaging::EReceiveUnstoredPhoneAck)
		{
		iWaitForCMTUnstored->Enable();

		if (iGlobals->iModemSupportsCMTIMode)  
		// CTMI parsing should be enabled if supported for receive unstored, 
		// phone ack mode. See GSM 07.05 section 3.4.1 and defect SHY-54GFHD
			{
			iWaitForCMTIStored->Enable();
			}
		}
					
	// Unpack parameters to lowest usable data type and then process them
	RMobileSmsMessaging::TMobileSmsGsmTpdu* pdu=STATIC_CAST(RMobileSmsMessaging::TMobileSmsGsmTpdu*,aParam1);
	UNPACK_PCKG(attr,aParam2,RMobileSmsMessaging::TMobileSmsReceiveAttributesV1);

 	iReceiveSmsQueue->PopAndCompleteClientWhenPossible(aReqHandle,pdu,&attr);
	return KErrNone;
	}

void CMobileSmsMessaging::CancelReceiveMessage()
/**
 * Called by ExtFunc to cancel a ReceiveMessage request
 */
	{
	LOCAL_LOGTEXT("CancelReceiveMessage","Enter function");
	iWaitForCMTIStored->Disable();
	iWaitForCMTUnstored->Disable();
	iReceiveSmsQueue->PopAndCompleteClientWhenPossibleCancel();
	}


TInt CMobileSmsMessaging::GetCaps(const TTsyReqHandle aTsyReqHandle,TDes8* aParam1)
/**
 * Called by ExtFunc. 
 */
	{
	// Unpack parameters to lowest usable data type and then process them
	UNPACK_PCKG(caps,aParam1,RMobileSmsMessaging::TMobileSmsCapsV1);
	switch(caps.ExtensionId())
		{
	case KETelExtMultimodeV1:		
		//
		// We are rely on the phone having been intiialised for the SMS receive caps
		// If the phone is not initialised complete with KErrNotReady
		if(iGlobals->iPhoneStatus.iInitStatus != EPhoneInitialised)
			{
			// Phone is not yet initialised
			ReqCompleted(aTsyReqHandle,KErrNotReady);
			}
		else
			{
			// Phone is initialised
			caps.iSmsMode=RMobileSmsMessaging::KCapsGsmSms;	// This TSY only supports GSM
			caps.iSmsControl=0;
			// Supports sending 
			caps.iSmsControl|=RMobileSmsMessaging::KCapsSendNoAck;	

			// If Phone supports AT+CSCA so say TSY supports SMSP list
			if(iGlobals->iModemSupportsCSCAMode)
				{
				caps.iSmsControl|=RMobileSmsMessaging::KCapsGetSmspList;
				caps.iSmsControl|=RMobileSmsMessaging::KCapsSetSmspList;
				}

			// The receive SMS mode capabiltiies were got during the initialisation of the TSY
			if(iGlobals->iModemSupportsCMTIMode)
				caps.iSmsControl|=RMobileSmsMessaging::KCapsReceiveStored;
			if(iGlobals->iModemSupportsCMTMode)
				caps.iSmsControl|=RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck;
		
			ReqCompleted(aTsyReqHandle, KErrNone);
			}
		break;

	default:
		LOCAL_LOGTEXT("GetCaps","Unknown parameters version");
		__ASSERT_DEBUG(EFalse,Panic(EMobileSmsMessagingUnknownParamVersion));	
		return KErrUnknown;
		}
    return KErrNone;
	}




TInt CMobileSmsMessaging::SendMessage(const TTsyReqHandle aTsyReqHandle,TDes8* aParam1,TDes8* aParam2)
/**
 * Called by ExtFunc to process EMobileSmsMessagingSendMessage IPC.
 */
  	{
	// Unpack parameters
	UNPACK_PCKG(msgAttr,aParam2,RMobileSmsMessaging::TMobileSmsSendAttributesV1);

	// Check if we can handle format of message
	// Try to be optomistic, only reject message if we know we can't handle it
	if(msgAttr.iFlags&RMobileSmsMessaging::KSmsDataFormat && 
	   msgAttr.iDataFormat!=RMobileSmsMessaging::EFormatUnspecified && 
       msgAttr.iDataFormat!=RMobileSmsMessaging::EFormatGsmTpdu)
		{
		LOCAL_LOGTEXT("SendMessage","Format of given message not supported");
		// Put extended error in upper 8-bits and core error in lower 8-bits
		TInt error = (KErrMMEtelSmsFormatNotSupported << 8) | KErrNotSupported;
		return error;
		}
	else
		{	
		// Start active object that will do the sending for us
		LOCAL_LOGTEXT("SendMessage","Starting CATSmsMessagingSend state machine");
		iATSmsMessagingSend->SetMsgAttributes(&msgAttr);
		iATSmsMessagingSend->ExecuteCommand(aTsyReqHandle,aParam1);
		}

	return KErrNone;
	}

//*********************************************************
//*		Enumerate Message Stores & Get Message Store Info
//***********************************************************

TInt CMobileSmsMessaging::EnumerateMessageStores(const TTsyReqHandle aTsyReqHandle,
												 TDes8* aParam1)
	{
	LOCAL_LOGTEXT("EnumerateMessageStores","Enter function");

	if (iGlobals->iPhoneStatus.iInitStatus != EPhoneInitialised)
	{
		ReqCompleted(aTsyReqHandle,KErrNotReady);

	return KErrNone;
	}

	UNPACK_PCKG(storeCount,aParam1,TInt);
	iATSmsMemoryStorage->CopyDataFromCATInit(iInit);
	storeCount = iATSmsMemoryStorage->GetNumberOfMessageStores();

	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CMobileSmsMessaging::GetMessageStoreInfo(const TTsyReqHandle aTsyReqHandle,
											  TDes8* aParam1, TDes8* aParam2)
	{
	LOCAL_LOGTEXT("GetMessageStoreInfo","Enter function");

	if (iGlobals->iPhoneStatus.iInitStatus != EPhoneInitialised)
	{
		ReqCompleted(aTsyReqHandle,KErrNotReady);

	return KErrNone;
	}

	UNPACK_PCKG(storeIndex,aParam1,TInt);
	
	TInt ret(KErrNone);
	ret=iATSmsMemoryStorage->WhichPreferredMem(storeIndex);
	if(ret==KErrNone)
		iATSmsMemoryStorage->Start(aTsyReqHandle,aParam2);

	return ret;
	}

//*******************************************
//*		Get SMSP List
//*******************************************
TInt CMobileSmsMessaging::GetSmspListPhase1(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2)
/** Get SMSP List Phase 1 
 *
 * If the GetSmspListPhase1L should leave this method takes care of that and 
 * makes a premature ReqCompleted to the client.
 *
 * @param aTsyReqHandle the request ID 
 * @param aClient The client sends down a handle that is saved together with the 
 *				  list so the list can be returned to the right client in phase 2.
 * @param aBufSiz The size of the retrieved network list. The size is set in 
 * @return error code. 
 */
	{
	LOCAL_LOGTEXT("GetSmspListPhase1","Enter function");

	UNPACK_PCKG(clientId,aParam1,RMobilePhone::TClientId);
	UNPACK_PCKG(bufSize,aParam2,TInt);

	TRAPD(leaveCode,GetSmspListPhase1L(aTsyReqHandle,clientId,bufSize));
	return leaveCode;
	}

																		
void CMobileSmsMessaging::GetSmspListPhase1L(TTsyReqHandle aTsyReqHandle, 
											 RMobilePhone::TClientId& aClient, 
											 TInt& aBufSize)
/** Get SMSP List Phase 1 
 *
 * Phase 1 retrieves the only parameter support - the service centre addess 
 * with help of AT+CSCA?, 
 * The memory space needed to store the list is then calculated 
 * and returned to the client.The returned list is saved to be 
 * copied to the client in phase 2, when the client has alocated memory for it.	
 * The actual execution of the AT command (AT+CSCA) is done in the CATSmsGetSCAddress class.
 *
 * @param aTsyReqHandle const pointer to the request ID 
 * @param aClient		The client sends down a handle that is saved together with the 
 *						list so the list can be returned to the right client in phase 2.
 * @param aBufSize		The size of the retrieved network. The size is set in CATSmsGetSCAddress::ParseResponseL().
 * @return error code. 
 */
	{
	LOCAL_LOGTEXT("GetSmspListPhase1L","Enter function");

	CListReadAllAttempt* read=CListReadAllAttempt::NewL(&aClient,aTsyReqHandle);
	CleanupStack::PushL(read);

	iSmspListInfo.iBufSize = &aBufSize;
	iSmspListInfo.iBufPtr = &(read->iListBuf);
	iGetSmspList->AppendL(read);

	iATSmsGetSCAddress->ExecuteCommand(aTsyReqHandle, &iSmspListInfo); 

	CleanupStack::Pop(read);
	}

TInt CMobileSmsMessaging::GetSmspListPhase2(const TTsyReqHandle aTsyReqHandle,
                                            TDes8* aParam1,TDes8* aParam2)
/** Get SMSP List phase 2 
 *
 * In this metod the list which was retrieved during phase 1 is copied to 
 * the memory which the client has allocated for this purose.
 * @param aTsyReqHandle		Const pointer to the request ID 
 * @param aClient			Handle to the client which list we are looking for.
 * @param aBuf				Pointer to the memory that the etelmm has allocated. 
 * @return error code. 
 */
	{	
	LOCAL_LOGTEXT("GetSmspListPhase2","Enter function");

	UNPACK_PCKG(clientId,aParam1,RMobilePhone::TClientId);
	
	CListReadAllAttempt* read=NULL;
	const TInt numberOfLists = iGetSmspList->Count();

	// Find the get SMSP attempt from this client  
	for (TInt i = 0; i < numberOfLists; ++i)
		{
		read = iGetSmspList->At(i);
		if ((read->iClient.iSessionHandle==clientId.iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==clientId.iSubSessionHandle))
			{

			//
			// We have to check that the listPtr is not NULL as if the phone/modem
			// responds with a blank service centre listPtr==NULL.
			CBufBase* listPtr=read->iListBuf;
			if(listPtr)
				{
				TPtr8 bufPtr(listPtr->Ptr(0));
				aParam2->Copy(bufPtr);						// Copy the streamed list to the client
				}

			delete read;
			iGetSmspList->Delete(i);
			ReqCompleted(aTsyReqHandle,KErrNone);	// Completes the retrieval of a network list succesfully.
			return KErrNone;
			}
		}

	Panic(EIllegalEvent); 
	return(KErrNotFound);
	}

TInt CMobileSmsMessaging::CompleteSmspListCancel(const TTsyReqHandle aTsyReqHandle)
/** CompleteSmspListCancel
 *
 * This method is called from the CATSmsGetSCAddress class if a cancel is succesful
 * It cleans-up the client's stored request data
 * @param aTsyReqHandle		Const pointer to the request ID 
 * @return error code. 
 */
	{
	LOGTEXT(_L8("CMobileSmsMessaging::GetSmspListCancel called"));
	// Remove the read all attempt from iGetSmspList
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetSmspList->Count(); ++i)
		{
		read = iGetSmspList->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetSmspList->Delete(i);
			break;
			}
		}
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileSmsMessaging::StoreSmspList(const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	LOGTEXT(_L8("CMobileSmsMessaging::StoreSmspList called"));
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, (void)ProcessStoreSmspListL(aTsyReqHandle, aBuffer););
	return leaveCode;
	};

TInt CMobileSmsMessaging::ProcessStoreSmspListL(TTsyReqHandle aTsyReqHandle, TDes8* aBuffer)
	{
	CMobilePhoneSmspList* smspList=CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
	
	smspList->RestoreL(*aBuffer);

	// We only support one entry (the default entry)
	if (smspList->Enumerate() != 1)
		ReqCompleted(aTsyReqHandle, KErrNotSupported);

	// Just get first entry - this is default entry contain SC address
	iSCEntry=smspList->GetEntryL(0);

	iATSmsSetSCAddress->ExecuteCommand(aTsyReqHandle,&(iSCEntry.iServiceCentre));

	CleanupStack::PopAndDestroy(); // smspList
	return KErrNone;
	}


TBool CMobileSmsMessaging::IsSupportedIPC(const TInt aIpc) const
	{
	switch(aIpc)
		{
	case EMobileSmsMessagingGetCaps:
	case EMobileSmsMessagingSetReceiveMode:
	case EMobileSmsMessagingGetReceiveMode:
	case EMobileSmsMessagingSendMessage:
	case EMobileSmsMessagingReceiveMessage:
	case EMobileSmsMessagingEnumerateMessageStores:
	case EMobileSmsMessagingGetMessageStoreInfo:
	case EMobileSmsMessagingGetSmspListPhase1:
	case EMobileSmsMessagingGetSmspListPhase2:
	case EMobileSmsMessagingStoreSmspList:
		return ETrue;

	default:
		return EFalse;
		}
	}
