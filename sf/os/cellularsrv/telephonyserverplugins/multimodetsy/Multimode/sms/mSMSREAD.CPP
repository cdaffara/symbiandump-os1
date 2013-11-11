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
// Reads an SMS message present on the ME (mobile equipment). AT+CMGR=xx 
// 
//

#include <et_phone.h>		// for TSY_HANDLE_INIT_VALUE

#include "panic.h"	// Panic codes
#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSMSREAD.H"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "mSMSSTOR.H"
#include "ATINIT.H"

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsMessagingRead::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif



CATSmsMessagingRead* CATSmsMessagingRead::NewL (CATIO* aIo, CTelObject* aTelObject,
												CATInit* aInit, CPhoneGlobals* aMmStatus)
	{
	CATSmsMessagingRead* self = new (ELeave) CATSmsMessagingRead(aIo, aTelObject, aInit, aMmStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsMessagingRead::CATSmsMessagingRead(CATIO* aIo, CTelObject* aTelObject,
										 CATInit* aInit, CPhoneGlobals* aMmStatus)
	:CATSmsCommands(aIo, aTelObject, aInit, aMmStatus),
	iSmsStore(reinterpret_cast<CMobileSmsStore*>(aTelObject)),
	iCancelled(EFalse)
	{}


void CATSmsMessagingRead::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)	
	{
	__ASSERT_DEBUG(aParams,Panic(EMobileSmsMessagingNullParameter));
	__ASSERT_DEBUG(aTsyReqHandle!=TSY_HANDLE_INIT_VALUE,Panic(EMobileSmsMessagingNullParameter));

	// Save the parameters into our class data
	iReqHandle = aTsyReqHandle;
	iCancelled=EFalse;
	iMsgEntry = reinterpret_cast<RMobileSmsStore::TMobileGsmSmsEntryV1*>(aParams);
	
	iMsgLocation=iMsgEntry->iIndex;

	iShortStoreName = iSmsStore->iStoreName;

	// Change iPhoneGlobals->iMmTsyEventSignalActive so system knows that our EventSignal
	// is active and requires sole use of the serial link to the phone.
	if (ComparePrefMem(iShortStoreName))
		{
		// the compared memories are the same, so we can 
		// start reading the message from the phone
		StartReadSequence();
		}
	else 
		{
		// the compared memories are different, must change 
		// phone mem to temporary mem.
		SetCurrentPrefMem(iShortStoreName);
		iState = EATWaitForSendingPrefMemComplete;
		}
	}


void CATSmsMessagingRead::StartReadSequence()
	{
	iTxBuffer.Format(KSmsReadCommand, iMsgLocation);
	iIo->Write(this, iTxBuffer);
	iIo->SetTimeOut(this, 5000);
	iState = EATWaitForSendingCMGRRequestComplete;
	}


void CATSmsMessagingRead::EventSignal(TEventSource aSource)
//
// Finite State Machine for handling events from the comm port
//
	{
	LOGTEXT2(_L8("CATSmsMessagingRead:\tiState = %D"), iState);
	if (aSource == ETimeOutCompletion)
		{
		LOGTEXT(_L8("CATSmsMessagingRead:\tTimeout Error during Sms Messaging Read"));
		iPhoneGlobals->iPhoneStatus.iModemDetected = RPhone::EDetectedNotPresent;
		RemoveStdExpectStrings();
		if ((iState != EATWaitForPrefMemResponse) && (!ComparePrefMem(iShortStoreName)))
			iPhoneGlobals->iPhonePrefMem = iShortStoreName;
		if (iCancelled)
			Complete(KErrCancel, aSource);
		else
			Complete(KErrTimedOut, aSource);
		return;
		}

	switch (iState)
		{
		case EATWaitForSendingPrefMemComplete:
			__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
				{
				iIo->WriteAndTimerCancel(this);
				StandardWriteCompletionHandler(aSource, 5);
				iState = EATWaitForPrefMemResponse;
				}
			break;

	case EATWaitForPrefMemResponse:
			__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
				{
				iIo->WriteAndTimerCancel(this);
				RemoveStdExpectStrings();
				TInt pos=iIo->BufferFindF(KCPMSResponseString);
				if (pos == KErrNotFound)
					{
					LOGTEXT(_L8("CATSmsMessagingRead:\tCould not set preferred memory to client's pref'd mem"));
					Complete(pos, aSource);
					return;
					}
				LOGTEXT(_L8("CATSmsMessagingRead:\tPhone's pref'd memory successfully set to client's pref'd mem"));
				iPhoneGlobals->iPhonePrefMem=iShortStoreName;
				if (iCancelled)
					Complete(KErrCancel,aSource);
				else
					StartReadSequence();
				}
			break;

	case EATWaitForSendingCMGRRequestComplete:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource, 50);
		AddCmsErrorExpectString();					// Defect fix 2/7/00, see "release.txt"
		iState=EATWaitForCMGRResponseComplete;
		break;
	
	case EATWaitForCMGRResponseComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			RemoveStdExpectStrings();
			RemoveCmsErrorExpectString();			// Defect fix 2/7/00, see "release.txt"
			TRAPD(ret, CMGRResponseL());

			if ((iMsgLength==0) && (ret==KErrNone))
				{
				LOGTEXT(_L8("CATSmsMessagingRead:\tThe Client has Read from a message location that is empty"));
				ret=KErrNotFound;
				}
			Complete(ret,aSource);
			}
		break;

	default:
		break;
		}
	}

void CATSmsMessagingRead::CMGRResponseL()
//
// Parse the +CMGR: message from the ME.
// It is of the form +CMGR: <Msg Status>,<Msg Length> <New line> PDU
//
	{
	ParseBufferLC();
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	iMsgLength = 0;

	entry=iter++;
	if((!entry)||(entry->iResultPtr!=KCMGRResponseString))
		User::Leave(KErrNotFound);

	LOGTEXT(_L8("CATSmsMessagingRead:\tFound +CMGR String!"));

// Read the message status
	entry=iter++;
	if(!entry)
		User::Leave(KErrGeneral);
	TLex8 msgStatLex((entry->iResultPtr).Ptr());
	TUint msgStat;
	(void)User::LeaveIfError(msgStatLex.Val(msgStat));

// Read the message length
	entry=iter++;
	if(!entry)
		User::Leave(KErrGeneral);
	if(entry->iResultPtr.Length()==0)
		{
		entry=iter++;
		if(!entry)
			User::Leave(KErrGeneral);
		}

	TLex8 msgLenLex((entry->iResultPtr).Ptr());
	(void)User::LeaveIfError(msgLenLex.Val(iMsgLength));

	entry=iter++;
	if(!entry)
		User::Leave(KErrGeneral);

	TSpecialPdu asciiPdu;
	asciiPdu = entry->iResultPtr;

	(void)User::LeaveIfError(CATSmsUtils::ConvertAsciiToBinary(asciiPdu,iPdu));
	TPtrC8 pduPtr(iPdu);

	if (iMsgLength*2 != asciiPdu.Length())
		{
		// There is an SCA prepended so remove this and store it in iGsmServiceCentre
		PopulateScaFieldsAndRemove(pduPtr,iGsmServiceCentre);
		}

	if(iMsgEntry)
		{
		iMsgEntry->iMsgData.Copy(pduPtr);
		iMsgEntry->iMsgStatus = GsmMsgStatusToStoreStatus(msgStat);
		}

	CleanupStack::PopAndDestroy();
	}


void CATSmsMessagingRead::PopulateScaFieldsAndRemove(TPtrC8& aPdu, RMobilePhone::TMobileAddress& aGsmServiceCentre)
//
// Populate the SCA field in the MM ETel structure from the SCA prepended in a received PDU.
//
	{
	const TUint8 KTONBitMask =0x70;
	const TUint8 KNPIBitMask=0x0f;
	const TUint8 KTONBitShift=4;
	const TUint8 KNPIBitShift=0;
	_LIT(KInternationalPrefix,"+");

	__ASSERT_ALWAYS(aPdu.Length()>0,Panic(ECMTHandlerDesPassedWithZeroLength));

	aGsmServiceCentre.iTypeOfNumber=(RMobilePhone::TMobileTON)0;
	aGsmServiceCentre.iNumberPlan=(RMobilePhone::TMobileNPI)0;
	aGsmServiceCentre.iTelNumber.Zero();
	
	TUint8 len=aPdu[0];
	
	if(len==0)
		{
		// A zero length SCA has been prepended - just strip this first byte off
		aPdu.Set(aPdu.Mid(len+1));
		return;
		}

	TUint8 numDes=aPdu[1];
	aGsmServiceCentre.iTypeOfNumber=(RMobilePhone::TMobileTON)((numDes&KTONBitMask)>>KTONBitShift);
	aGsmServiceCentre.iNumberPlan=(RMobilePhone::TMobileNPI)((numDes&KNPIBitMask)>>KNPIBitShift);

	if(aGsmServiceCentre.iTypeOfNumber==RMobilePhone::EInternationalNumber)
		aGsmServiceCentre.iTelNumber.Append(KInternationalPrefix);

	TInt i;
	TUint16 digit;
	for(i=2;i<(len+1);i++)
		{
		digit=(TUint16)((aPdu[i]&0x0f)+0x30);
		aGsmServiceCentre.iTelNumber.Append(digit);
		digit=(TUint16)(((aPdu[i]&0xf0)>>4)+0x30);
		if(digit==0x003f)		// 'F' is the padding digit at the end of a number
			break;
		aGsmServiceCentre.iTelNumber.Append(digit);
		}
	aPdu.Set(aPdu.Mid(len+1));
	}

void CATSmsMessagingRead::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	LOCAL_LOGTEXT("Stop","Enter function");
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		

	LOGTEXT2(_L8("Current state TSY is cancelling from %d"), iState);
	// Wait for convenient place in state machine to cancel
	iCancelled=ETrue;
	}

void CATSmsMessagingRead::Complete(TTsyReqHandle aReqHandle,TInt aError)
	{
	iReqHandle=aReqHandle;
	Complete(aError, EReadCompletion);
	}

void CATSmsMessagingRead::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	iPhoneGlobals->iEventSignalActive = EFalse;
	if (iReqHandle != 0)
		iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();

	//
	// Call our base classes Complete so that
	// we allow it do what ever it needs to do.
	CATCommands::Complete(aError,aSource);

	LOCAL_LOGTEXT("Complete","Exit function");
	}


void CATSmsMessagingRead::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iMsgLocation=NULL;
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

RMobileSmsStore::TMobileSmsStoreStatus CATSmsMessagingRead::GsmMsgStatusToStoreStatus(TInt aMsgStat)
/** 
 * This method converts a GSM message status to Multimode message satus. 
 */
	{
	switch (aMsgStat)
		{
		case 0: // The SMS is received but unread
			return RMobileSmsStore::EStoredMessageUnread;
		case 1: // The SMS is received and read
			return RMobileSmsStore::EStoredMessageRead;
		case 2: // The SMS is stored but unsent
			return RMobileSmsStore::EStoredMessageUnsent;
		case 3: // The SMS is stored and Sent
			return RMobileSmsStore::EStoredMessageSent;

		default: // All other status values
			return RMobileSmsStore::EStoredMessageUnknownStatus;
		}
	}

