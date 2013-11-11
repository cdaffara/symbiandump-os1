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

#include <etelmm.h>				// for RMobileSmsMessaging::TMobileSmsReceiveAttributesV1
#include "mSLOGGER.H"			// for LOGTEXT2 and other logging utilities
#include "ATIO.H"			
#include "Matstd.h"
#include "cmti_stored.h"		// header file for this cpp file
#include "sms_rx_queue.h"		// for CReceiveSmsQueue

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsWaitForAndHandleCMTIStored::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSmsWaitForAndHandleCMTIStored* CATSmsWaitForAndHandleCMTIStored::NewL(CATIO* aIo,CTelObject* aTelObject, CPhoneGlobals* aGlobals,
									                                     CReceiveSmsQueue& aReceiveQueue)
	{
	CATSmsWaitForAndHandleCMTIStored* wait=new(ELeave) CATSmsWaitForAndHandleCMTIStored(aIo,aTelObject,aGlobals,aReceiveQueue);
	// Currently no need for a ConstructL call
	return wait;
	}

CATSmsWaitForAndHandleCMTIStored::CATSmsWaitForAndHandleCMTIStored(CATIO* aIo, CTelObject* aTelObject,
												   CPhoneGlobals* aGlobals, CReceiveSmsQueue& aReceiveQueue) 
	:CATBase(aIo,aTelObject,aGlobals)
	,iReceiveQueue(aReceiveQueue)
	{}


CATSmsWaitForAndHandleCMTIStored::~CATSmsWaitForAndHandleCMTIStored()
	{
	iIo->RemoveExpectStrings(this);		
	// We do not need to NULL iCMTExpectString and iCDSExpectString
	// as this object is about to de destroyed
	}

void CATSmsWaitForAndHandleCMTIStored::Enable()
	{
	LOCAL_LOGTEXT("Enable","Enabled listening for CMTI");
		
	if (!iMsgArrivedExpectString)
		iMsgArrivedExpectString=iIo->AddExpectString(this,KCMTIMatchString);
	}

void CATSmsWaitForAndHandleCMTIStored::Disable()
	{
	LOCAL_LOGTEXT("Disable","Disabled listening for CMTI");
	iIo->RemoveExpectStrings(this);		
	iMsgArrivedExpectString=NULL;
	}


void CATSmsWaitForAndHandleCMTIStored::EventSignal(TEventSource aSource)
/**
 * Called when a '+CMTI:' is received.
 * Pushes the received messages details (store name & index) into the CReceiveSmsQueue.
 */
	{
	LOCAL_LOGTEXT("EventSignal","Enter function");

	TInt ret(KErrNone);
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 rxAttr;
		
	__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
	__ASSERT_ALWAYS(iPhoneGlobals, Panic(EIllegalNullPtrParameter));
	TRAP(ret, CMTIResponseL(rxAttr));
	if(ret!=KErrNone)
		{
		LOCAL_LOGTEXT("EventSignal","Unable to parse CMTI response");
		}
	else
		{
		//
		// Add the received SMS's details in the CReceiveSmsQueue object
		LOCAL_LOGTEXT("EventSignal","Adding SMS details to iReceiveQueue");
		RMobileSmsMessaging::TMobileSmsGsmTpdu nullPdu;
		nullPdu.Zero();
		iReceiveQueue.Push(nullPdu,rxAttr);
		
		// If no AT command is currently running then we have to prod the CReceiveSmsQueue
		// to try and read PDUs from the phone.
		// This is because the CReceiveSmsQueue object only attempts to read PDUs
		// from the phone just after an AT command completes (see CATCommands::Complete).
		// There may not be another AT command completed during the life
		// of the TSY, so we have to prod CReceiveSmsQueue so that it can complete
		// any outstanding ReadMessage reqeuests.	
		if(!iPhoneGlobals->iEventSignalActive)
			{
			LOCAL_LOGTEXT("EventSignal","Prodding CReceiveSmsQueue object");

			// Allow the CReceiveSmsQueue object to read PDUs from the phones memory, if needed
			if(iPhoneGlobals->iReceiveSmsQueuePtr)
				iPhoneGlobals->iReceiveSmsQueuePtr->ReadPDUFromPhone();
			}			
		else
			{
			LOCAL_LOGTEXT("EventSignal","Can not prod CReceiveSmsQueue object as iEventSignalActive");
			}

		// We are done, but there is no client request to complete
		// we just return and wait until we get our next CMTI command
		}
	}

void CATSmsWaitForAndHandleCMTIStored::CMTIResponseL(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr)
	{
	// +CMTI: <type>, <index>
	//        string,  number

	ParseLineLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	//
	// Validate we did actually receive the CMTI string
	entry=iter++;
	if (!entry || entry->iResultPtr != KCMTIResponseString)
		User::Leave(KErrGeneral);

	//
	// Parse and store the store name where the message is stored
	entry=iter++;
	if (!entry)
		User::Leave(KErrGeneral);
	CATSmsUtils::ConvertStoreNameToEtelMMVersion(aAttr.iStore,entry->iResultPtr);

	//
	// Parse and store the index in the store where the message has been stored
	entry=iter++;
	if (!entry)
		User::Leave(KErrGeneral);
	TLex8 lex(entry->iResultPtr);
	(void)User::LeaveIfError(lex.Val(aAttr.iStoreIndex));

	{
	//
	// For logging purposes we need a 8 bit descriptor 
	TBuf8<RMobilePhone::KMaxMobileNameSize> buf;
	buf.Copy(aAttr.iStore);
	LOCAL_LOGTEXT("CMTIResponseL","Have parsed CMTI response string");
	LOGTEXT3(_L8("Index=%d  Store=%S"),aAttr.iStoreIndex,&buf);
	}

	CleanupStack::PopAndDestroy();	// PopAndDestory object pushed by ParseLineLC
	}


void CATSmsWaitForAndHandleCMTIStored::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	LOCAL_LOGTEXT("CompleteWithIOError","Enter function");
	// CATIO removes expect strings in event of IO error so don't do it here
	iMsgArrivedExpectString=NULL;	
	// inform client of error so that it does not continue to wait for incoming SMS
	// now that this wait object has been disabled
	iReceiveQueue.CompleteClientReqWithError(aStatus);
	}

