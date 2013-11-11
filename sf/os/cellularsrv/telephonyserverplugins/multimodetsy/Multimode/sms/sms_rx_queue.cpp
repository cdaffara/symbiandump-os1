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
// Class defintition for CReceiveSmsQueue
// 
//

/**
 @file
*/

#include <etelmm.h>				// for RMobileSmsMessaging...
#include <et_phone.h>           // for TSY_HANDLE_INIT_VALUE
#include "mSMSMESS.H"			// for CMobileSmsMessaging...
#include "sms_rx_queue.h"		// header for this cpp file
#include "mSLOGGER.H"			// for LOGTEXT2...
#include "sms_rx_queue_pduread.h"		// for CATSmsReadPDU

// Macros
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CReceiveSmsQueue::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CReceiveSmsQueue* CReceiveSmsQueue::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aGlobals,CMobileSmsMessaging& aMobileSmsMessaging)
/** First stage constructor
 */
 	{
	CReceiveSmsQueue* self=new(ELeave) CReceiveSmsQueue(aMobileSmsMessaging,*aGlobals);
	CleanupStack::PushL(self);	
	self->ConstructL(aIo,aTelObject,aInit,aGlobals);
	CleanupStack::Pop(self);
	return self;
	}

CReceiveSmsQueue::CReceiveSmsQueue(CMobileSmsMessaging& aMobileSmsMessaging,CPhoneGlobals& aGlobals)
	: iGlobals(aGlobals), iMobileSmsMessaging(aMobileSmsMessaging)
 /** C++ constructor
  */
	{}

void CReceiveSmsQueue::ConstructL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aGlobals)
/** Second stage constructor
 */
	{
	// Create the AT command object which we will use to read PDUs from the 
	// phones memory
	iATReadPDU=CATSmsReadPDU::NewL(aIo,aTelObject,aInit,aGlobals,*this);
	}

CReceiveSmsQueue::~CReceiveSmsQueue()
/** C++ destructor
 */
	{
	delete iATReadPDU;
	}



void CReceiveSmsQueue::Push(const RMobileSmsMessaging::TMobileSmsGsmTpdu& aPdu, 
			      		    const RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr)
	{
	LOCAL_LOGTEXT("Push","Pushing item to queue");
	LOGTEXT3(_L8("iQueueFront=%d  iQueueBack=%d"),iQueueFront,iQueueBack);
		
	if(QueueFull())
		{
		LOCAL_LOGTEXT("Push","Queue overflow!");
		// Allow message to drop of front of queue
		iQueueFront=(iQueueFront+1)%KReceiveSmsQueueSize;
		}

	// Copy item into queue
	(iQueue[iQueueBack]).iPdu=aPdu;
	(iQueue[iQueueBack]).iAttr=aAttr;

	// Update pointer
	iQueueBack=(iQueueBack+1)%KReceiveSmsQueueSize;

	// Try to complete client request in case one was outstanding
	CompleteClientReqIfPossible();
	}


void CReceiveSmsQueue::PopAndCompleteClientWhenPossible(const TTsyReqHandle aReqHandle,
                                                        RMobileSmsMessaging::TMobileSmsGsmTpdu* aPdu, 
									                    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1* aAttr)
	{
	__ASSERT_DEBUG(iClientReqOutstanding==EFalse,Panic(EReceiveSmsQueueRequestOutstanding));

	//
	// Store clients request details
	iClientReqOutstanding=ETrue;
	iClientReq=aReqHandle;
	iClientReqPdu=aPdu;
	iClientReqAttr=aAttr;

	// Try to complete client request
	CompleteClientReqIfPossible();
	}											

void CReceiveSmsQueue::PopAndCompleteClientWhenPossibleCancel()
	{
	LOCAL_LOGTEXT("PopAndCompleteClientWhenPossibleCancel","Enter function");
	LOGTEXT3(_L8("iQueueFront=%d  iQueueBack=%d"),iQueueFront,iQueueBack);
	
	if(iClientReqOutstanding)
		{
		// Complete client request with KErrCancel
		iMobileSmsMessaging.ReqCompleted(iClientReq,KErrCancel);
		iClientReqOutstanding=EFalse;
		}
	}

void CReceiveSmsQueue::ReadPDUFromPhone()
/**
 * ReadPDUFromPhone should be called by CATCommands::Complete at which time the 
 * TSY base classes guarantee that there is not another AT command which is running.
 * ReadPDUFromPhone starts the reading of the PDU of the message at the front of
 * the queue if we do not already store it's PDU.
 */
	{
	LOCAL_LOGTEXT("ReadPDUFromPhone","Enter function");

	//
	// Check if we need to read PDU from phone
	if(QueueEmpty())
		{
		LOCAL_LOGTEXT("ReadPDUFromPhone","Queue is empty, will not read PDU");
		return;
		}

	if((iQueue[iQueueFront]).iPdu.Length()!=0)
		{
		LOCAL_LOGTEXT("ReadPDUFromPhone","Already have PDU, will not read PDU");
		return;
		}

	LOCAL_LOGTEXT("ReadPDUFromPhone","Need to read a PDU from the phone...");

	//
	// Read PDU from phone
	// (We can call ExecuteCommand with a 0 TSY handle as we are guaranteed, by 
	// the CATBase base class that another AT command is not running, and therefore 
	// the CATBase will not try to complete this request with KErrInUse 
	// due to iEventSignalActive==ETrue)
	iATReadPDU->ExecuteCommand(0,&(iQueue[iQueueFront]));
	}


void CReceiveSmsQueue::CompleteClientReqIfPossible()
/**
 * If there is an oustanding client request then this function
 * will check to see if the client request can be completed.
 * If a client request can be completed then this function will 
 * complete the client request by popping an item from the queue, 
 * passing the item data to the client and then calling ReqCompleted
 */	
	{
	LOCAL_LOGTEXT("CompleteClientReqIfPossible","Enter function");
	
	//
	// We can only complete a client request if...
	//   The client has made a request
	//   Our Queue is not empty
	//   We have the PDU for the message at the front of the queue
	if(iClientReqOutstanding && !QueueEmpty() && (iQueue[iQueueFront]).iPdu.Length()!=0)
		{
		LOCAL_LOGTEXT("CompleteClientReqIfPossible","Popping item from queue");

		// Copy item from queue
		*iClientReqPdu=(iQueue[iQueueFront]).iPdu;
		*iClientReqAttr=(iQueue[iQueueFront]).iAttr;
		
		// Update pointer
		iQueueFront=(iQueueFront+1)%KReceiveSmsQueueSize;

		// Complete client request
		iMobileSmsMessaging.ReqCompleted(iClientReq,KErrNone);
		iClientReqOutstanding=EFalse;
		}
	}

TBool CReceiveSmsQueue::QueueEmpty()
/** Class Private Utility function
 */
	{
	return iQueueFront==iQueueBack;
	}

TBool CReceiveSmsQueue::QueueFull()
/** Class Private Utility function
 */
	{
	return ((iQueueBack+1)%KReceiveSmsQueueSize)==iQueueFront;
	}

void CReceiveSmsQueue::CompleteClientReqWithError(TInt aError)
/** Inform client that there is a problem waiting for messages
 */
	{
	if(iClientReqOutstanding)
		{
		LOGTEXT2(_L8("CReceiveSmsQueue::CompleteClientReqWithError - Error %d"),aError);
		iMobileSmsMessaging.ReqCompleted(iClientReq,aError);
		iClientReqOutstanding=EFalse;
		}
	}
