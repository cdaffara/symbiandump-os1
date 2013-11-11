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
#include "cmt_unstored.h"		// header file for this cpp file
#include "sms_rx_queue.h"		// for CReceiveSmsQueue

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsWaitForAndHandleCMTUnstored::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSmsWaitForAndHandleCMTUnstored* CATSmsWaitForAndHandleCMTUnstored::NewL(CATIO* aIo,CTelObject* aTelObject, CPhoneGlobals* aGlobals,
									                                     CReceiveSmsQueue& aReceiveQueue)
	{
	CATSmsWaitForAndHandleCMTUnstored* wait=new(ELeave) CATSmsWaitForAndHandleCMTUnstored(aIo,aTelObject,aGlobals,aReceiveQueue);
	// Currently no need for a ConstructL call
	return wait;
	}

CATSmsWaitForAndHandleCMTUnstored::CATSmsWaitForAndHandleCMTUnstored(CATIO* aIo, CTelObject* aTelObject,
												   CPhoneGlobals* aGlobals, CReceiveSmsQueue& aReceiveQueue) 
	:CATBase(aIo,aTelObject,aGlobals)
	,iReceiveQueue(aReceiveQueue)
	{}


CATSmsWaitForAndHandleCMTUnstored::~CATSmsWaitForAndHandleCMTUnstored()
	{
	iIo->RemoveExpectStrings(this);	
	// We do not need to NULL iCMTExpectString and iCDSExpectString
	// as this object is about to de destroyed
	}

void CATSmsWaitForAndHandleCMTUnstored::Enable()
	{
	LOCAL_LOGTEXT("Enable","Enabled listening for CMT & CDS");

	if (iState==ENoState)
		{
		iState=EWaitForCMT;
		
		// Add +CMT:* expect string
		if (!iCMTExpectString)
			iCMTExpectString=iIo->AddExpectString(this,KCMTMatchString);

		// Add +CDS:* expect string
		if (!iCDSExpectString)
			iCDSExpectString=iIo->AddExpectString(this,KCDSMatchString);
		}
	}

void CATSmsWaitForAndHandleCMTUnstored::Disable()
	{
	LOCAL_LOGTEXT("Disable","Disabled listening for CMT & CDS");
	
	if (iState!=EWaitForCmtSecondLine)
		{
		iIo->RemoveExpectStrings(this);		
		iCMTExpectString=NULL;
		iCDSExpectString=NULL;
		iState=ENoState;
		}
	else
		{
		// wait to receive message we're half-way through before disabling
		iState=EWaitForCmtSecondLineThenDisable;
		}
	}


void CATSmsWaitForAndHandleCMTUnstored::EventSignal(TEventSource aSource)
/**
 * Called when a '+CMT:' or '+CDS:' is received.
 * +CMT: [<alpha>],<length><CR><LF><pdu>
 * +CDS: <length><CR><LF><pdu>
 *
 * This is a two state machine that first waits for the +CMT: [<alpha>],<length><CR><LF>
 * and then waits for the final <pdu>
 *
 * When a PDU has been read from the phone, it is pushed into the CReceiveSmsQueue.
 */
	{
	TInt ret(KErrNone);
	iIo->WriteAndTimerCancel(this);
	LOCAL_LOGTEXT("EventSignal","Enter function");
		
	switch(iState)
		{
	case EWaitForCMT:
		LOCAL_LOGTEXT("EventSignal","EWaitForCMT");
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
		
		TRAP(ret,ParseFirstCMTLineL(iPduLen));
		if(ret!=KErrNone)
			{
			LOCAL_LOGTEXT("EventSignal","Failed to parse 1st line");
			// No need for more error handling - just wait for next CMT
			}
		else
			{
			//
			// Add the expect string * so that we receive whatever is on the next line
			// (the conditional expression below ensures no memory leak if "+CMT:"
			// is found on consecutive lines)
			if(!iWaitForAnything)
				iWaitForAnything=iIo->AddExpectString(this,KAsterisk);
		
			//
			// We need to keep the buffered data so we...
			iIo->SetPreserveBufferFlag(ETrue);			

			//
			// Wait for the reception of the second line
			iState=EWaitForCmtSecondLine;
			}
		break;

	case EWaitForCmtSecondLine:
	case EWaitForCmtSecondLineThenDisable:

		LOCAL_LOGTEXT("EventSignal","EWaitForCmtSecondLine");
		__ASSERT_ALWAYS(aSource == EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		iIo->RemoveExpectString(iWaitForAnything);
		iWaitForAnything=NULL;
		iIo->SetPreserveBufferFlag(EFalse);			// We've got the PDU, and will delete the buffer when the line is parsed

		{		// Curly brackets used to scope pdu and attr local data
		//
		// Parse and Push received message into CReceiveSmsQueue
		RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 attr;
		TRAPD(ret,ParseSecondCMTLineL(pdu,attr));
		if(ret!=KErrNone)
			{
			LOCAL_LOGTEXT("EventSignal","Failed to parse 2nd line");
			}
		else
			{
			LOCAL_LOGTEXT("EventSignal","Adding SMS to iReceiveQueue");
			iReceiveQueue.Push(pdu,attr);
			}
		}

		//
		// We are done, but there is no client request to complete
		// we just return and wait until we get our next CMT command
		// unless we should disable now
		if (iState==EWaitForCmtSecondLineThenDisable)
			Disable();
		else
			iState=EWaitForCMT;
		break;

	case ENoState:
	default:
		// We should never get here, but we don't need to panic in release code.
		__ASSERT_DEBUG(EFalse,Panic(EATWaitForCMTNoState));
		break;
		} 
	}



void CATSmsWaitForAndHandleCMTUnstored::ParseFirstCMTLineL(TInt& aPduLen)
/**
 * Parse 1st line of CMT response, of the format: "+CMT: [<alpha>], <pdu_len>"
 * Stores the <pdu_len> in aPduLen
 */
	{
	ParseLineLC();

	CATParamListEntry* entry;
	CATParamListEntry* alpha;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	entry=iter++;
	if (entry==NULL || (entry->iResultPtr!=KCMTResponseString && entry->iResultPtr!=KCDSResponseString))
		{
		LOCAL_LOGTEXT("ParseFirstCMTLineL","Failed to find response +CMT: or +CDS:");
		User::Leave(KErrGeneral);
		}

	//
	// Get the <alpha> token, optional this token may return the message length token
	alpha=iter++;
	if(alpha==NULL)
		{
		LOCAL_LOGTEXT("ParseFirstCMTLineL","Failed with 1st argument");
		User::Leave(KErrGeneral);
		}

	//
	// Get the message length token
	entry=iter++;
	if(entry==NULL)
		entry=alpha;	// The alpha token must have been missing and thus alpha holds the message length

	if(entry==NULL)		
		{
		LOCAL_LOGTEXT("ParseFirstCMTLineL","Failed with 2nd argument");
		User::Leave(KErrGeneral);
		}

	//
	// Process the message length
	aPduLen=CATParamListEntry::EntryValL(entry);
	LOGTEXT2(_L8("New SMS detected of length %d"),iPduLen);

	CleanupStack::PopAndDestroy();		// ParseLineLC pushed object
	}


void CATSmsWaitForAndHandleCMTUnstored::ParseSecondCMTLineL(RMobileSmsMessaging::TMobileSmsGsmTpdu& aPdu,RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& aAttr)
/**
 * Parse second line of CMT or DS response, of the format: "<pdu>"
 * The <pdu> is saved to aPdu. If the <pdu> is found to have a prefixed SCA
 * then that is removed before the save to aPdu.
 */
	{
	LOCAL_LOGTEXT("ParseSecondCMTLineL","Enter funciton");

	// Intiailise the attributes
	aAttr.iFlags=0;
	//
	// Copy the received pdu to local storage so that we can process
	// and modify it.
	// Buffer size calculated as follows;
	//   (max pdu size + max prefixed SCA size) * number of ASCII chars used to code an octet
	TBuf8<(RMobileSmsMessaging::KGsmTpduSize+12)*2> localPdu;	
	{
	ParseLineLC();
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;			// Have to increment iter so it returns a pointer
	if(entry==NULL)
		User::Leave(KErrGeneral);		// Would prefer not to leave and return an error
		                                // but this function has to be trapped as we 
										// have the ParseLineLC() call.
	localPdu.Copy(entry->iResultPtr);
	CleanupStack::PopAndDestroy();		// ParseLineLC pushed object
	}

	// Nokia 9210 sometimes aborts the CMT frame in the middle of pdu.
	// Worst, it is sometimes contains odd number of nibbles which
	// causes panic in ConvertAsciiToBinary() method. This type of
	// frames will simply be discarded.
	if(localPdu.Length()<(iPduLen*2))
		User::Leave(KErrUnderflow);
	
	//
	// Check if we have a prefixed SCA on our pdu.
	// If we do then remove it.
	if(localPdu.Length()>(iPduLen*2))
		{
		if(CATSmsUtils::ReadAndRemoveAddressFromAscii(localPdu,aAttr.iGsmServiceCentre,ETrue)!=KErrNone)
			{
			LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to read and remove SCA from PDU");
			}
		else
			{
			aAttr.iFlags|=RMobileSmsMessaging::KGsmServiceCentre;
			}
		}

	//
	// Convert received pdu from ASCII into binary
	if (CATSmsUtils::ConvertAsciiToBinary(localPdu,aPdu)!=KErrNone)
		{
		LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to code PDU into binary");
		}
	else
		{
		aAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
		aAttr.iFlags|=RMobileSmsMessaging::KSmsDataFormat;
		aAttr.iStatus=RMobileSmsMessaging::EMtMessageUnstoredPhoneAck;
		aAttr.iFlags|=RMobileSmsMessaging::KIncomingStatus;
		}
	}	
	


void CATSmsWaitForAndHandleCMTUnstored::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	LOCAL_LOGTEXT("CompleteWithIOError","Enter function");
	// CATIO removes expect strings in event of IO error so don't do it here
	iCMTExpectString=NULL;			
	iCDSExpectString=NULL;			
	// inform client of error so that it does not continue to wait for incoming SMS
	// now that this wait object has been disabled
	iReceiveQueue.CompleteClientReqWithError(aStatus);
	}

