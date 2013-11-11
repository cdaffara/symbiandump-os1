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
// This writes an SMS message and stores the message location number. AT+CMGW=xx
// 
//

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSMSWRIT.H"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "mSMSSTOR.H"
#include "smsutil.h"
#include "et_phone_util.h"

_LIT8(KSmsWriteLengthCommand,"AT+CMGW = %d\r");
_LIT8(KSmsWriteCommand,"AT+CMGW = %d,%d\r");

CATSmsMessagingWrite* CATSmsMessagingWrite::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aMmStatus)
	{
	CATSmsMessagingWrite* self = new(ELeave) CATSmsMessagingWrite(aIo, aTelObject, aInit, aMmStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsMessagingWrite::CATSmsMessagingWrite(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aMmStatus)
	:CATSmsCommands(aIo, aTelObject, aInit, aMmStatus)
	,iExpectString(NULL)
	,iSmsEntry(NULL)
	,iCancelled(EFalse)
	,iSmsStore(reinterpret_cast<CMobileSmsStore*>(aTelObject))
	{
	}

CATSmsMessagingWrite::~CATSmsMessagingWrite()
	{}

void CATSmsMessagingWrite::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	__ASSERT_DEBUG(aParams,Panic(EMobileSmsMessagingNullParameter));
	__ASSERT_DEBUG(aTsyReqHandle!=TSY_HANDLE_INIT_VALUE,Panic(EMobileSmsMessagingNullParameter));

	// Save the parameters into our class data
	iReqHandle = aTsyReqHandle;
	iCancelled=EFalse;
	iSmsEntry = reinterpret_cast<RMobileSmsStore::TMobileGsmSmsEntryV1*>(aParams);

	iNextAttempt=iPhoneGlobals->iPhoneTestState;
	// If undefined, try old standard first
	if(iNextAttempt==CPhoneGlobals::EPhoneTestUndefined)
		iNextAttempt=CPhoneGlobals::EPhoneTestOldStandard;

	iPhoneGlobals->iEventSignalActive = ETrue;
	if (ComparePrefMem(iSmsStore->iStoreName))
		StartPduWrite();
	else // Compared memories are different
		{
		SetCurrentPrefMem(iSmsStore->iStoreName);
		iState = EATWaitForSendingPrefMemComplete;
		}
	}

void CATSmsMessagingWrite::EventSignal(TEventSource aSource)
	{
	LOGTEXT2(_L8("CATSmsMessagingWrite:\tiState = %D"), iState);

	if (aSource == ETimeOutCompletion)
		{
		LOGTEXT(_L8("CATSmsMessagingWrite:\tTimeout Error during Sms Messaging Write"));
		iPhoneGlobals->iPhoneStatus.iModemDetected = RPhone::EDetectedNotPresent;
		RemoveStdExpectStrings();
		if ((iState != EATWaitForPrefMemResponse)&&!ComparePrefMem(iSmsStore->iStoreName))
			iPhoneGlobals->iPhonePrefMem = iSmsStore->iStoreName; 
		Complete(KErrTimedOut, aSource);
		return;
		} //if

	switch(iState)
		{
	// the following 3 cases are special cases, occuring only when the PhonePrefMem != ClientPrefMem
	case EATWaitForSendingPrefMemComplete:
		//
		// Finished writing preferred memory, now wait for response
		//
			{
			HandleWriteCompletion(aSource);
			iState = EATWaitForPrefMemResponse;
			}
		break;

	case EATWaitForPrefMemResponse:
		//
		// Got response from writing preferred memory.  Validate response, and if
		// successful, then start the write sequence
		//
			{	
			TInt ret=HandleResponseCompletion(aSource,EFalse);
			if (ret!=KErrNone)
				{
				Complete(ret,aSource);
				return;
				}		
			TInt pos=iIo->BufferFindF(KCPMSResponseString);
			if (pos == KErrNotFound)
				{
				Complete(pos, aSource);
				return;
				}
			LOGTEXT(_L8("CATSmsMessagingWrite:\tPhone's preferred memory successfully set to client's preferred memory."));
			iPhoneGlobals->iPhonePrefMem=iSmsStore->iStoreName;
			if (iCancelled)
				Complete(KErrCancel,aSource);
			else
				StartPduWrite();
			}
		break;

	case EATWaitForSendingPduLengthComplete:
		//
		// Finished sending pdu length command, now wait for response
		//
			{
			HandleWriteCompletion(aSource);
			iExpectString=iIo->AddExpectString(this,KSmsEnterPduModeResponse,ETrue); // Expecting "> "
			iIo->SetTimeOut(this, 5000);
			iState=EATReadPduEnterPduModeResponseCompleted;
			}
		break;
	
	case EATReadPduEnterPduModeResponseCompleted:
		//
		// Received a response to the pdu length command.  Validate, and if successful
		// send the actual pdu in ascii form
		//
			{
			TInt ret=HandleResponseCompletion(aSource,EFalse);
			if (ret!=KErrNone)
				{
				Complete(ret,aSource);
				return;
				}		
			ret=ConvertCMSErrorToKErr(CMSErrorValue());
			iIo->RemoveExpectString(iExpectString);
			iExpectString=NULL;
			if(ret)
				{
				Complete(ret,aSource);
				return;
				}
			iMsgDataAscii.Append(KCtrlZChar); // Adding the CTRL-Z that indicates the end of the PDU
			iIo->Write(this,iMsgDataAscii);
			iIo->SetTimeOut(this, 5000);
			iState=EATWaitForSendingPduComplete;
			}
		break;

	case EATWaitForSendingPduComplete:
		//
		// Finished writing pdu, wait for a response
		//
			{
			HandleWriteCompletion(aSource);
			iState=EATReadSmsRefNumCompleted;
			}
		break;

	case EATReadSmsRefNumCompleted:
		//
		// Received response to pdu write, validate and if successful set preferred
		// memory back to original value if it was changed for this command,
		// otherwise on failure try the new format if the current attempt used the
		// old ETSI format
		//
			{
			TInt ret=HandleResponseCompletion(aSource,EFalse);
			if (ret!=KErrNone)
				{
				Complete(ret,aSource);
				return;
				}		
			TInt err=ConvertCMSErrorToKErr(CMSErrorValue());
			
			if (err==KErrNone)
			 	TRAP(err,ParseResponseL());
			
			if(err!=KErrNone)
				{ 
				if (iCancelled)
					Complete(KErrCancel,aSource);
				else
					{	
					// Try new standard if old standard failed
					LOGTEXT2(_L8("CATSmsMessagingWrite:\tFailed with code %d"),err);
					if (iNextAttempt==CPhoneGlobals::EPhoneTestOldStandard)
						{
						iNextAttempt=CPhoneGlobals::EPhoneTestNewStandard;
						StartPduWrite();
						}
					else
						// Must have tried new standard and this failed too
						Complete(err);
					}
				}
			else
				{
				// If new standard worked then store this is as default for this phone
				if (iNextAttempt==CPhoneGlobals::EPhoneTestNewStandard)
					iPhoneGlobals->iPhoneTestState=CPhoneGlobals::EPhoneTestNewStandard;
				Complete(KErrNone,aSource);
				}
			}
		break;


	case EATCancellingWaitForWriteComplete:
		//
		// Finished writing cancel request, now wait for response
		//
			{
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			AddStdExpectStrings();
			iIo->SetTimeOut(this);
			iState=EATCancellingReadCompleted;
			}
		break;

	case EATCancellingReadCompleted:
		//
		// Cancel command response received
		//
			{
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			iIo->WriteAndTimerCancel(this);
			Complete(KErrCancel, aSource);
			}
		break;

	default:
		break;
		}
	}

void CATSmsMessagingWrite::StartPduWrite()
/**
 * Kicks off the writing of the actual pdu. Begins by converting the pdu
 * into ascii form, and writing the pdu length and message status to the phone.
 */
	{
	iMsgDataAscii.Zero();

	// Convert PDU from binary to ASCII
	CATSmsUtils::AppendDataToAscii(iMsgDataAscii,iSmsEntry->iMsgData);
	if(iMsgDataAscii.Length()==0)
		{
		LOGTEXT(_L8("CATSmsMessagingWrite:\tStartPduWrite - Failed to convert binary PDU to ASCII"));
		Complete(KErrCorrupt);
		return;
		}

	// Send PDU length to the phone
	const TInt pduLengthSemiOctets(iMsgDataAscii.Length());
	const TInt pduLengthOctets(pduLengthSemiOctets/2+pduLengthSemiOctets%2);

	if (iNextAttempt==CPhoneGlobals::EPhoneTestNewStandard)
 		{
		// Prepend a zero length SCA to PDU - this forces phone to use default SCA
		// See GSM 07.05 for details
		_LIT8(zeroLengthSca, "00");
		iMsgDataAscii.Insert(0,zeroLengthSca);
		}
	
	iTxBuffer.Zero();
	TInt stat=SetMsgStatus();
	if (stat==KErrNotSupported)
		iTxBuffer.Format(KSmsWriteLengthCommand,pduLengthOctets);
	else
		iTxBuffer.Format(KSmsWriteCommand,pduLengthOctets,stat);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,5000);
	iState=EATWaitForSendingPduLengthComplete;
	}

void CATSmsMessagingWrite::Complete(TInt aError)
	{
	Complete(aError, EReadCompletion);
	}

void CATSmsMessagingWrite::Complete(TInt aError, TEventSource aSource)
	{
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	iExpectString = NULL;
	iPhoneGlobals->iEventSignalActive = EFalse; 
	if (iReqHandle != 0)
		iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource == EWriteCompletion)
		iIo->Read();

	//
	// Call our base classes Complete so that
	// we allow it do what ever it needs to do.
	CATCommands::Complete(aError,aSource);

	iState = EATNotInProgress;
	}

void CATSmsMessagingWrite::ParseResponseL()
//
// Parse the message reference number and return in smsEntry.iIndex
//
	{
	LOGTEXT(_L8("CATSmsMessagingWrite:\tRetrieving message reference number"));
	iBuffer.Set(iIo->Buffer());
	TInt pos = iBuffer.FindF(KCMGWResponseString);
	if (pos == KErrNotFound)
		{
		LOGTEXT(_L8("CATSmsMessagingWrite:\tError - Cannot find '+CMGW:' string"));
		pos = iBuffer.FindF(KErrorString);
		if (pos != KErrNotFound)
			User::Leave(KErrGeneral);
		User::Leave(pos);
		}
	pos += 6;
	
	// Place the message reference number into buffer (ie: everything after +CMGW: string)
	TInt woop = iBuffer.Length()-pos;
	iBuffer.Set(iBuffer.Right(woop));
	
	TLex8 yyLex(iBuffer);
	yyLex.Inc();			// steps over the ':' Should always do this....NEVER falls over!
	yyLex.SkipSpace();
	TUint val;
	if(yyLex.Val(val)==KErrNone) // Grab the message reference number.
		{
		iSmsEntry->iIndex = val;
		}
	else
		{
		LOGTEXT(_L8("CATSmsMessagingWrite:\tError. Invalid Message Reference Number."));
		User::Leave(KErrGeneral);
		}
	}

void CATSmsMessagingWrite::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	LOGTEXT(_L8("CATSmsMessagingWrite:\tCancelling Sms Write"));

	if ((iState==EATWaitForSendingPduLengthComplete) ||
		(iState==EATReadPduEnterPduModeResponseCompleted) ||
		(iState==EATWaitForSendingPduComplete) ||
		(iState==EATReadSmsRefNumCompleted))
		{
		// Attempt to cancel write command if we can
		RemoveStdExpectStrings();
		iIo->WriteAndTimerCancel(this);
		iTxBuffer.Format(KEscapeChar);
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this);
		iState = EATCancellingWaitForWriteComplete;
		}
	else
		// Wait until safe point to cancel the write
		iCancelled=ETrue;
	}

void CATSmsMessagingWrite::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

TInt CATSmsMessagingWrite::SetMsgStatus()
//
// Sets the message status
//
	{
	TInt status;
	switch(iSmsEntry->iMsgStatus)
		{
		case (RMobileSmsStore::EStoredMessageUnread): 
			status = 0;
		break;
		
		case (RMobileSmsStore::EStoredMessageRead):
			status = 1;
		break;

		case (RMobileSmsStore::EStoredMessageUnsent):
			status = 2;
		break;

		case (RMobileSmsStore::EStoredMessageSent):  
		case (RMobileSmsStore::EStoredMessageDelivered):   
			status = 3;
		break;

		default:
			status = KErrNotSupported;
		break;
		}
	return status;
	}

