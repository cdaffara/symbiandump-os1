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
// Implementation of CATSmsMessagingSend.
// 
//

/**
 @file
*/


#include <etelmm.h>

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSMSSEND.H"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "smsutil.h"	// for CATSmsUtils

//
// Constants
//
const TInt KPduMode=0;

//
// Macros
//

#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsMessagingSend::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif

/**
 * CANCEL_AND_RETURN_IF_NEEDED
 * Used to implement cancellation at safe points inside CATSmsMessagingSend::EventSignal
 */
#define CANCEL_AND_RETURN_IF_NEEDED()\
{\
if(iStop)\
	{\
	Complete(KErrCancel);\
	LOCAL_LOGTEXT("CANCEL_AND_RETURN_IF_NEEDED","Cancelled");\
	return;\
	}\
}


//
// Class Implementation
//
CATSmsMessagingSend* CATSmsMessagingSend::NewL(	CATIO* aIo,
												CTelObject* aTelObject,
												CATInit* aInit,
												CPhoneGlobals* aGsmStatus)
/**
 * Creates a new instance of CATSmsMessagingSend
 */
	{
	CATSmsMessagingSend* self=new(ELeave) CATSmsMessagingSend(aIo,aTelObject,aInit,aGsmStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsMessagingSend::CATSmsMessagingSend(	CATIO* aIo,
											CTelObject* aTelObject,
											CATInit* aInit,
											CPhoneGlobals* aGsmStatus)
	:CATSmsCommands(aIo,aTelObject,aInit,aGsmStatus)
/**
 * C++ constructor
 */
{}

void CATSmsMessagingSend::ConstructL()
/**
 * 2nd phase contructor
 */
	{
	CATCommands::ConstructL();
	iMsgDataAscii.Zero();			// Just in case
	}

CATSmsMessagingSend::~CATSmsMessagingSend()
/**
 * C++ destructor
 */
	{
	delete iMsgData;
	}

void CATSmsMessagingSend::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
/**
 * Starts the procedure of sending an SMS
 * @param aTsyReqHandle Handle to client, to be completed when operation done
 * @param aParams Pointer to the SMS PDU (with or without SC prefixed) to be sent 
 */
	{
	LOCAL_LOGTEXT("Start","Have been requested to send a message");
	
	// Intialize data 
	iHaveRetriedWithOtherPduStd=EFalse;
	iStop=EFalse;

	// ::SetMsgAttributes must be called before ::Start is called
	__ASSERT_DEBUG(iMsgAttributes,Panic(EATSmsMessagingSendNullMsgAttributes));		

	// Save access to the data we be given thru our parameters
	iReqHandle=aTsyReqHandle;
	delete iMsgData;		// Ensure we don't orphan some memory
	iMsgData=((TDesC8*)aParams)->Alloc();
	if(!iMsgData)
		Complete(KErrNoMemory);

	// Kick off first AT stuff
	LOCAL_LOGTEXT("Start","Setting phone to PDU mode (as opposed to text mode)");
	iTxBuffer.Format(KSmsFormatCommand,KPduMode);
	WriteTxBufferToPhone(ESetPhoneToPDUMode);
	}


void CATSmsMessagingSend::StartFindSCA()
/**
 * Attempt to find an SCA to use when sending message.
 * The following SCA sources are checked in this order...
 *    Supplied by client in iMsgAttributes.iSc
 *    Use default SCA in the phone's memory
 *	  Use default SCA in the phone's memory after doing 'AT+CRES=1'
 */
	{
	LOCAL_LOGTEXT("StartFindSCA","");

	// Did client supply SCA in iMsgAttributes.iSc
	if(iMsgAttributes->iFlags&RMobileSmsMessaging::KGsmServiceCentre && iMsgAttributes->iGsmServiceCentre.iTelNumber.Length()!=0)
		{
		iMsgSCA=iMsgAttributes->iGsmServiceCentre;
		DoneFindSCA();
		}

	// Does the phone have a default SCA in memory
	else
		{
		TInt ret=CATSmsCommands::RequestATCommand(CATSmsCommands::EGetSCAFromPhone);
		if(ret!=KErrNone)
			Complete(ret);
		}
	
	}


void CATSmsMessagingSend::DoneFindSCA()
	{
	LOCAL_LOGTEXT("DoneFindSCA","");
	// At this point in the execution we can guarantee that...
	//	iMsgData holds the PDU to be sent in binary format without a prepended SCA
	//	iMsgSCA holds the SCA to be used to send the PDU

	// Check we can handle the Type-Of-Address of the SCA
	if(!(iMsgSCA.iNumberPlan==RMobilePhone::EIsdnNumberPlan &&
	   (iMsgSCA.iTypeOfNumber==RMobilePhone::EInternationalNumber ||
		iMsgSCA.iTypeOfNumber==RMobilePhone::EUnknownNumber)))
		{
		LOCAL_LOGTEXT("DoneFindSCA","SCA type unsupported");
		Complete(KErrCorrupt);
		return;
		}

	// Check which ETSI standard we think phone conforms to
	if(PhoneUsesNewPDUStandard())
		SendMessageToNewPhone();
	else
		SendMessageToOldPhone();
	}


void CATSmsMessagingSend::SendMessageToOldPhone()
/**
 * Send message to phone which uses new ETSI standard (no SCA prefixed to PDU expected,
 * use phone's default SCA setting instead).
 */
 	{
	LOCAL_LOGTEXT("SendMessageToOldPhone","");
	// Set the SCA to use as the default in the phone's memory
	iRequestSCA=iMsgSCA;
	TInt ret=CATSmsCommands::RequestATCommand(CATSmsCommands::ESetSCAInPhone);
	if(ret!=KErrNone)
		Complete(ret);
	}


void CATSmsMessagingSend::SendMessageToOldPhone_Stage2()
	{
	// Convert PDU from binary to ASCII
	iMsgDataAscii.Zero();
	CATSmsUtils::AppendDataToAscii(iMsgDataAscii,*iMsgData);
	if(iMsgDataAscii.Length()==0)
		{
		LOCAL_LOGTEXT("SendMessageToOldPhone_Stage2","Failed to convert binary PDU to ASCII");
		Complete(KErrCorrupt);
		return;
		}

	// Send PDU length to the phone
	const TInt pduLengthSemiOctets(iMsgDataAscii.Length());
	const TInt pduLengthOctets(pduLengthSemiOctets/2+pduLengthSemiOctets%2);
	iTxBuffer.Format(KSmsSendPduLengthCommand,pduLengthOctets);
	WriteTxBufferToPhone(ESendPDULengthToPhone);
	}

void CATSmsMessagingSend::SendPDUToPhone()
	{
	LOCAL_LOGTEXT("SendPDUToPhone","");
	// Send PDU to phone 
	iTxBuffer.Format(iMsgDataAscii);
	iTxBuffer.Append(KCtrlZChar);
	WriteTxBufferToPhone(ESendPDUToPhone);
	}

void CATSmsMessagingSend::SendPDUToPhone_Stage2()
	{
	// Get the message reference number & submit report and then we've finished 
	Complete(ParseCMGSResponse());
	}

void CATSmsMessagingSend::SendMessageToNewPhone()
/**
 * Send message to phone which uses new ETSI standard (SCA prefixed to PDU expected).
 */
	{
	LOCAL_LOGTEXT("SendMessageToNewPhone","");

	// Convert SCA to ASCII (ensure that 03.40 format is used to create SCA)
	iMsgDataAscii.Zero();
	if(CATSmsUtils::AppendAddressToAscii(iMsgDataAscii,iMsgSCA,ETrue)!=KErrNone || iMsgDataAscii.Length()==0)
		{
		LOCAL_LOGTEXT("SendMessageToNewPhone","Failed to prepend SCA to PDU");
		Complete(KErrCorrupt);
		return;
		}
	
	// Convert PDU to ASCII
	const TInt msgDataAsciiLen(iMsgDataAscii.Length());
	CATSmsUtils::AppendDataToAscii(iMsgDataAscii,*iMsgData);
	if(iMsgDataAscii.Length()==msgDataAsciiLen)
		{
		LOCAL_LOGTEXT("SendMessageToNewPhone","Failed to convert binary PDU to ASCII");
		Complete(KErrCorrupt);
		return;
		}

	// Send PDU length to the phone
	const TInt pduLengthSemiOctets(iMsgDataAscii.Length()-msgDataAsciiLen);	// Must not include the prefixed SCA in calculation
	const TInt pduLengthOctets(pduLengthSemiOctets/2+pduLengthSemiOctets%2);
	iTxBuffer.Format(KSmsSendPduLengthCommand,pduLengthOctets);
	WriteTxBufferToPhone(ESendPDULengthToPhone);
	}


void CATSmsMessagingSend::EventSignal(TEventSource aSource)
/**
 * Handle the events from the comm port
 *
 * This code is first called after the phone has been set to PDU mode.
 * First we find the SCA that we are going to use. 
 * Secondly we send the message to the phone depending on whether the phone seems to be using 
 * the new ETSI format (SCA prefixed to PDU expected) or the old ETSI format (no SCA prefixed
 * to PDU expected).
 *
 * @param aSource denotes if event is due to read, write or timeout
 */
	{
	LOCAL_LOGTEXT("EventSignal","");
	LOGTEXT3(_L8("iState=%D iSource=%D"),iState,aSource);
	LOGTEXT2(_L8("iStop=%D"),iStop);

	// Check to see if this class or our base class need to process the event
	if(CATSmsCommands::RequestATActive())
		{
		//
		// Allow base class to handle event, and find out if a request completed
		//
		CATSmsCommands::EventSignal(aSource);
		const CATSmsCommands::TRequest reqCompleted(CATSmsCommands::RequestATCompleted());

		if(reqCompleted!=ENone)
			CANCEL_AND_RETURN_IF_NEEDED();

		// Otheriwse, Check if a request completed as a result of the event processing
		switch(reqCompleted)
			{
		case CATSmsCommands::EGetSCAFromPhone:
			LOCAL_LOGTEXT("EventSignal","EGetSCAFromPhone completed");
			if(iRequestError==KErrNone)
				{
				iMsgSCA=iRequestSCA;
				DoneFindSCA();
				}
			else
				{
				LOCAL_LOGTEXT("EventSignal","Failed to find an SCA to use");
				Complete(iRequestError,aSource);
				}
			break;

		case CATSmsCommands::ESetSCAInPhone:
			LOCAL_LOGTEXT("EventSignal","ESetSCAInPhone completed");
			if(iRequestError==KErrNone)
				SendMessageToOldPhone_Stage2();
			else
				{
				LOCAL_LOGTEXT("EventSignal","Failed to set SCA in phone");
				Complete(iRequestError,aSource);
				}
			break;

		case CATSmsCommands::ENone:	// Must not be caught by default case 
			break;
		default:
			LOCAL_LOGTEXT("EventSignal","CATSmsCommands unknown request completed");
			__ASSERT_DEBUG(EFalse,Panic(EATSmsMessagingUnknownRequestCompleted));
			}
		}
	else
		{
		//
		// This class will handle event
		//
		if (aSource==ETimeOutCompletion)
			{
			LOCAL_LOGTEXT("EventSignal","Timeout error");
			iIo->WriteAndTimerCancel(this);
			Complete(KErrTimedOut, aSource);
			}

		TInt ret(KErrNone);
		switch(iState)
			{
		case ESetPhoneToPDUMode:
			if(aSource==EWriteCompletion)
				HandleWriteCompletion(aSource);		
			else
				{
				ret=HandleResponseCompletion(aSource,EFalse);
				if (ret!=KErrNone)
					{
					Complete(ret,aSource);
					return;
					}		
				StartFindSCA();
				}
			break;

		case ESendPDULengthToPhone:
			if(aSource==EWriteCompletion)
				{
				HandleWriteCompletion(aSource);
				iExpectString=iIo->AddExpectString(this,KSmsEnterPduModeResponse,ETrue);
				}
			else
				{
				ret=HandleResponseCompletion(aSource,EFalse);
				if (ret!=KErrNone)
					{
					Complete(ret,aSource);
					return;
					}		
				ret=ConvertCMSErrorToKErr(CMSErrorValue());
				iIo->RemoveExpectString(iExpectString);
				iExpectString=NULL;
				if (ret!=KErrNone)
					{
					Complete(ret,aSource);
					return;
					}		

				// If we get a Cancel request just before we send the PDU, we instead send an escape 
				// character to trigger a PDU send failure and then continue as normal.
				if (iStop)
					{
					LOCAL_LOGTEXT("EventSignal","Cancel requested. Sending Escape Character instead of PDU");
					
					// In case of Ericsson phone, after sending the escape char we are not receiving expected
					// response (OK/ERROR), it is just echoing those char. But if the escape char is prefixed
					// by "at", then it responds with expected response.

					// But in case of other phones, we believe the escape char alone works.  We have confirmed
					// this with Nokia phones.

					_LIT16(KEricsson,"*ERICSSON*");
					if(iPhoneGlobals->iPhoneId.iManufacturer.MatchF(KEricsson)==0)
						{
						_LIT8(KEscapeSequenceString,"at%S\r");
						iTxBuffer.Format(KEscapeSequenceString,&KEscapeChar);
						}
					else
						{
						iTxBuffer.Format(KEscapeChar);
						}
					WriteTxBufferToPhone(ESendEscapeCharToPhone);
					}
				else
					SendPDUToPhone();
				}
			break;
		
		case ESendPDUToPhone:
			if(aSource==EWriteCompletion)
				{
				HandleWriteCompletion(aSource);
				}
			else
				{
				const TInt err=ConvertCMSErrorToKErr(CMSErrorValue());
				ret=HandleResponseCompletion(aSource,EFalse);
				if (ret!=KErrNone)
					{
					Complete(ret,aSource);
					return;
					}		

				// Check if an error occurred
				if(err!=KErrNone)
					{
					// If we have not already, then retry using the other Pdu standard
					if(!iHaveRetriedWithOtherPduStd)
						{
						CANCEL_AND_RETURN_IF_NEEDED();
						// Retry with new standard
						LOCAL_LOGTEXT("EventSignal","Failed to send message, will retry using other phone standard");
						iHaveRetriedWithOtherPduStd=ETrue;
						TogglePhonePDUStandard();
						DoneFindSCA();
						}
					else
						{
						// We have tried both PDU standards and have failed :-(
						LOCAL_LOGTEXT("EventSignal","Failed to send message :-(");
						Complete(err,aSource);
						}
					}
				else
					{
					// Success, we have sent the message ;-)
					if (iStop)	{LOCAL_LOGTEXT("EventSignal","Message send successful, Cancel request to late, PDU already sent");}
					else		{LOCAL_LOGTEXT("EventSignal","Message send successful ;-)");}
					SendPDUToPhone_Stage2();
					}
				}
			break;
		
		case ESendEscapeCharToPhone:
			if(aSource==EWriteCompletion)
				{
				HandleWriteCompletion(aSource);
				}
			else
				{
				ret=HandleResponseCompletion(aSource,EFalse);
				if (ret!=KErrNone)
					{
					Complete(ret,aSource);
					return;
					}
				Complete(KErrCancel,aSource);
				}
			break;

		case ENotInProgress: // Required to stop 'unhandled enum' warning with ARM4
			break;
			}
		}
	}


void CATSmsMessagingSend::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","");
	LOGTEXT3(_L8("aError=%D aSource=%D"),aError,aSource);

	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();
	iState = ENotInProgress; 
	}

void CATSmsMessagingSend::SetMsgAttributes(RMobileSmsMessaging::TMobileSmsSendAttributesV1* aMsgAttributes)
	{
	iMsgAttributes=aMsgAttributes;
	}

void CATSmsMessagingSend::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	LOCAL_LOGTEXT("Stop","Client has requested cancel");
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));

	// Ensure our base class notes that a cancel has been requested
	CATSmsCommands::RequestATCommandCancel();

	// Set our flag to denote we should cancel as soon as possible
	iStop=ETrue;
	}

void CATSmsMessagingSend::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=ENotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = ENotInProgress;
		}
	}

//
// Utility functions 
//
TBool CATSmsMessagingSend::PhoneUsesNewPDUStandard()
/**
 * This code assumes that EPhoneTestOldStandard and EPhoneTestUndefined are 
 * the same.
 */
 	{
	return (iPhoneGlobals->iPhoneTestState==CPhoneGlobals::EPhoneTestNewStandard);
	}

void CATSmsMessagingSend::TogglePhonePDUStandard()
/**
 * This code assumes that EPhoneTestOldStandard and EPhoneTestUndefined are 
 * the same.
 */
	{
	if(iPhoneGlobals->iPhoneTestState==CPhoneGlobals::EPhoneTestNewStandard)
		iPhoneGlobals->iPhoneTestState=CPhoneGlobals::EPhoneTestOldStandard;		
	else
		iPhoneGlobals->iPhoneTestState=CPhoneGlobals::EPhoneTestNewStandard;		
	}

void CATSmsMessagingSend::WriteTxBufferToPhone(TState aNewState)
/**
 * Sends the contents of iTxBuffer to the phone
 */
 	{
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,KATWriteTimeout);
	iState=aNewState;
	}


TInt CATSmsMessagingSend::ParseCMGSResponse()		
/**
 * Parse CMGS response string for Message Reference Number &
 * SUBMIT-REPORT PDU (optional) and store their values in the clients data space.
 *
 * @return Standard KErr... values
 */
	{
	__ASSERT_DEBUG(iMsgAttributes,Panic(EATSmsMessagingSendNullMsgAttributes));		

	iBuffer.Set(iIo->Buffer());
	TInt pos=iBuffer.FindF(KCMGSResponseString);
	if (pos==KErrNotFound)
		{
		LOCAL_LOGTEXT("ParseCMGSResponse","Cannot find '+CMGS:' string");
		return KErrNotFound;
		}
	
	// Locate the message reference number
	// (ie. read in all digits form the first found to the end of the string)
	const TInt bufLength=iBuffer.Length();
	pos+=KCMGSResponseStringLength;
	while(pos<bufLength && !(TChar(iBuffer[pos]).IsDigit()))
		++pos;
	if(pos>=bufLength)
		{
		LOCAL_LOGTEXT("ParseCMGSResponse","Cannot find any digits after '+CMGS:'");
		return KErrNotFound;
		}

	// Read message number and store in clients data structure
	TPtrC8 ptr=iBuffer.Mid(pos);	
	TLex8 lex(ptr);
	TUint16 val;
	TInt ret=lex.Val(val,EDecimal);
	if(ret!=KErrNone)
		{
		LOCAL_LOGTEXT("ParseCMGSResponse","Unable to read Message Reference Number");
		return ret;
		}
	LOGTEXT2(_L8("CATSmsMessagingSend    Message reference number %d"),val);
	iMsgAttributes->iMsgRef=val;
	iMsgAttributes->iFlags|=RMobileSmsMessaging::KMessageReference;

	// Locate SUBMIT-REPORT (it does not have to exist)
	pos=iBuffer.FindF(KCommaChar);
	if(pos!=KErrNotFound)
		{
		while(pos<bufLength && !(TChar(iBuffer[pos]).IsHexDigit()))
			++pos;

		if(pos<bufLength)
			{
			// We have found a SUBMIT-REPORT PDU, so save it to clients data space
			LOCAL_LOGTEXT("ParseCMGSResponse","Found SUBMIT-REPORT PDU");

			iMsgAttributes->iSubmitReport.Zero();
			while(pos<bufLength && TChar(iBuffer[pos]).IsHexDigit())
				{
				iMsgAttributes->iSubmitReport.Append(TChar(iBuffer[pos]));
				++pos;
				}
			iMsgAttributes->iFlags|=RMobileSmsMessaging::KGsmSubmitReport;
			}
		}

	return KErrNone;
	}


