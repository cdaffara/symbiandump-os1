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


#include <et_phone.h>		// for TSY_HANDLE_INIT_VALUE

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSLOGGER.H"		// for LOGTEXT and other logging STUFF
#include "ATIO.H"
#include "mSMSSTOR.H"
#include "ATINIT.H"
#include "sms_rx_queue_pduread.h"		// header file for this CPP


#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsReadPDU::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSmsReadPDU* CATSmsReadPDU::NewL (CATIO* aIo, CTelObject* aTelObject,
												CATInit* aInit, CPhoneGlobals* aGlobals,
												CReceiveSmsQueue& aClient)
/** First stage construction
 */
	{
	CATSmsReadPDU* self = new (ELeave) CATSmsReadPDU(aIo,aTelObject,aInit,aGlobals,aClient);
	CleanupStack::PushL(self);
	self->ConstructL();			// Calls thru to base class
	CleanupStack::Pop();
	return self;
	}

CATSmsReadPDU::CATSmsReadPDU(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit, CPhoneGlobals* aGlobals,CReceiveSmsQueue& aClient)
	:CATSmsCommands(aIo, aTelObject, aInit, aGlobals),
	 iClient(aClient)
/** C++ constructor
 */
	{}


void CATSmsReadPDU::Start(TTsyReqHandle /*aTsyReqHandle*/, TAny* aParams)	
/**
 * Virtual from CATSmsCommands.
 * This method called by CATBase::ExecuteCommand, therefore iEventSignalActive 
 * flow control is handled for us.
 *
 * Note that this class is not started as the result of a TSY client request but
 * started internally to the TSY. As a result the aTsyReqHandle will be 0 and invalid.
 * This class should never complete a client request.
 */
 	{
	LOCAL_LOGTEXT("Start","Enter function");
	__ASSERT_DEBUG(aParams,Panic(ESmsRxQueuePduReadNullParameter));

	// Save the parameter
	iClientItem=static_cast<CReceiveSmsQueue::TItem*>(aParams);

	// Request that the ETEL Server buffer up requests from the client
	// so that we can process them later, as opposed to them all completing with 
	// KErrInUse.
	iTelObject->FlowControlSuspend();


	// Send the first AT command to phone
	// Request that the phone select use the appropriate memory store

	// Have to convert the unicode string to a 8bit string.
	// KMaxMobileNameSize from EtelMM so this buffer will be big enough
	// for what we need.
//	TBuf8<RMobilePhone::KMaxMobileNameSize> buf8;		
	CATSmsUtils::ConvertStoreNameToPhoneVersion(iClientItem->iAttr.iStore,iStoreName);
	iTxBuffer.Format(KSetPrefMemCommandWithString,&iStoreName,&iStoreName); 
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,KATWriteTimeout);
	iState=ESetPhonesPrefMemory;
	}


void CATSmsReadPDU::EventSignal(TEventSource aSource)
/**
 * Finite State Machine for handling events from the comm port
 * This class should never complete a client request.
 */
	{
	LOCAL_LOGTEXT("EventSignal","Enter function");

	//
	// Handle the timeout cases.
	if (aSource==ETimeOutCompletion)
		{
		LOCAL_LOGTEXT("EventSignal","Timeout occurred");
		RemoveStdExpectStrings();
		iState=ENoState;

		iPhoneGlobals->iPhonePrefMem = iStoreName; 

		Complete(KErrNone,aSource);// We have completed our AT command processing
		return;
		}

	//
	// Handle all other (non-timeout) cases.
	switch(iState)
		{
	case ESetPhonesPrefMemory:
		if(aSource==EWriteCompletion)
			HandleWriteCompletion(aSource);		
		else
			{
			if(HandleResponseCompletion(aSource,ETrue)!=KErrNone)
				{
				LOCAL_LOGTEXT("EventSignal","Phone returned an error, unable to read SMS");
				Complete(KErrNone,aSource);	// We have completed our AT command processing
				break;
				}

			//
			// Now try to read SMS from index we need to
			iPhoneGlobals->iPhonePrefMem = iStoreName; 
			iTxBuffer.Format(KSmsReadCommand,iClientItem->iAttr.iStoreIndex); 
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KATWriteTimeout);
			iState=EReadSMSFromPhone;
			}
		break;

	case EReadSMSFromPhone:
		if(aSource==EWriteCompletion)
			{
			HandleWriteCompletion(aSource);		
			// In addition to the standard OK and ERROR we need to listen for +CMGR:
			__ASSERT_DEBUG(!iCMGRExpectString,Panic(ESmsRxQueuePduReadNonNullExpectString));
			iCMGRExpectString=iIo->AddExpectString(this,KCMGRResponseString); 
			}
		else
			{
			// Remove our special expect string
			__ASSERT_DEBUG(iCMGRExpectString,Panic(ESmsRxQueuePduReadNullExpectString));
			if(iCMGRExpectString)
				{
				iIo->RemoveExpectString(iCMGRExpectString);	
				iCMGRExpectString=NULL;
				}

			if(HandleResponseCompletion(aSource,ETrue)!=KErrNone)
				{
				iTxBuffer.Format(KSmsReadCommand,iClientItem->iAttr.iStoreIndex); 
				iIo->Write(this,iTxBuffer);
				iIo->SetTimeOut(this,KATWriteTimeout);

				/*
				LOCAL_LOGTEXT("EventSignal","Phone returned an error, unable to read SMS");
				Complete(KErrNone,aSource);// We have completed our AT command processing
				*/
				break;
				}

			//
			// Parse response from phone and save data in iPdu
			TRAP_IGNORE(ParseAndStoreCMGRResponseL());
	
			//
			// Prod our client, in case they were waiting for us to read the PDU
			// so that they could complete a client ReceiveMessage request.
			iClient.CompleteClientReqIfPossible();

			//
			// We have finished, so intialise data ready for when our ::Start is 
			// called again
			iState=ENoState;
			iClientItem=NULL;		// We do not own this object so we do not have to delete it
	
			// We have completed our AT command processing
			Complete(KErrNone,aSource);
			}
		break;
		
	case ENoState:
		// We should never get here, but we don't need to panic in release code.
		__ASSERT_DEBUG(EFalse,Panic(ESmsRxQueuePduReadNoState));
		break;
		}
	}


void CATSmsReadPDU::Stop(TTsyReqHandle /*aReqHandle*/)
/**
 * Attempts to halt the process.
 * As this class is started internally to the TSY and does not have a 
 * client request handle, this Stop method should never be called. 
 */
	{
	LOCAL_LOGTEXT("Stop","Enter function");
	// This method should not be called, but there is no need to panic in release builds.
	__ASSERT_DEBUG(EFalse,Panic(ESmsRxQueuePduReadStopCalled));		
	}


void CATSmsReadPDU::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);

	// Allow base to do it's stuff
	CATSmsCommands::Complete(aError,aSource);

	// We do not call ReqComplete as we do not have a client request to be completed

	// Request that Etel Server stop buffering client IPC calls for us
	iTelObject->FlowControlResume(); 
	}


void CATSmsReadPDU::CompleteWithIOError(TEventSource aSource,TInt /*aStatus*/)
/**
 * This method has to be implemented as it is pure virtual in a base class.
 */
 	{
	Complete(KErrGeneral,aSource);
	}


void CATSmsReadPDU::ParseAndStoreCMGRResponseL()
/**
 * Parse the +CMGR: message from the ME.
 * It is of the form +CMGR: <Msg Status>,[<alpha>],<Msg Length> <New line> PDU
 */
	{
	LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Enter function");

	__ASSERT_DEBUG(iClientItem,Panic(ESmsRxQueuePduReadNullParameter));

	// Intiailise the attributes
	iClientItem->iAttr.iFlags=0;
	iClientItem->iAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
	iClientItem->iAttr.iFlags|=RMobileSmsMessaging::KSmsDataFormat;
	iClientItem->iAttr.iStatus=RMobileSmsMessaging::EMtMessageStored;
	iClientItem->iAttr.iFlags|=RMobileSmsMessaging::KIncomingStatus;
	iClientItem->iAttr.iFlags|=RMobileSmsMessaging::KStorageLocation;

	ParseBufferLC();
	CATParamListEntry* start;
	CATParamListEntry* msgAlpha;
	CATParamListEntry* msgLen;
	CATParamListEntry* msgPdu;

	{		// Curly brackets used to scope 'iter' variable
	//
	// Read in tokens
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	start=iter++;
	iter++;	// To skip past the msgStatus pointer which is not used.
	msgAlpha=iter++;
	msgLen=iter++;
	msgPdu=iter;
	}

	// If alpha token was missing then we have to reorder our tokens
	if(msgPdu==NULL)
		{
		LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","<alpha> token seems to be missing");
		msgPdu=msgLen;
		msgLen=msgAlpha;
		}

	// Ensure that we have all the tokens we need
	if(start==NULL || msgLen==NULL || msgPdu==NULL)
		{
		LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to read in tokens");
		CleanupStack::PopAndDestroy();		// destroy ParseBufferLC object
		return;
		}

	//
	// Process tokens		

	// Process message length token
	TInt msgLenVal(0);
	msgLenVal=CATParamListEntry::EntryValL(msgLen);

	//
	// Copy the received pdu to local storage so that we can process
	// and modify it.
	// Buffer size calculated as follows;
	//   (max pdu size + max prefixed SCA size) * number of ASCII chars used to code an octet
	TBuf8<(RMobileSmsMessaging::KGsmTpduSize+12)*2> localPdu;	
	localPdu.Copy(msgPdu->iResultPtr);

	//
	// Check if we have a prefixed SCA on our pdu.
	// If we do then remove it.
	if(localPdu.Length()>(msgLenVal*2))
		{
		if(CATSmsUtils::ReadAndRemoveAddressFromAscii(localPdu,iClientItem->iAttr.iGsmServiceCentre,ETrue)!=KErrNone)
			{
			LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to read and remove SCA from PDU");
			}
		else
			{
			iClientItem->iAttr.iFlags|=RMobileSmsMessaging::KGsmServiceCentre;
			}
		}

	//
	// Read originator address from PDU
	// We know from ETSI standards that the originator address 
	// will start with the 3rd character (at index 2) in the pdu
	if(CATSmsUtils::ReadAddressFromAscii(localPdu.Mid(2),iClientItem->iAttr.iOriginator,EFalse)!=KErrNone)
		{
		LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to read originator from PDU");
		}
	else
		{
		iClientItem->iAttr.iFlags|=RMobileSmsMessaging::KRemotePartyInfo;
		}

	//
	// Convert received pdu from ASCII into binary
	if(CATSmsUtils::ConvertAsciiToBinary(localPdu,iClientItem->iPdu)!=KErrNone)
		{
		LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Failed to code PDU into binary");
		}

	//
	// We've finsihed, do clean up
	CleanupStack::PopAndDestroy();		// destroy ParseBufferLC object
	LOCAL_LOGTEXT("ParseAndStoreCMGRResponseL","Exit function, PDU read OK");
	}
