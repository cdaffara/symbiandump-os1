// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file atsmsreceive.cpp
// This contains CAtSmsReceive which received incoming messages
// 

//system include
#include <etelmm.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>

//user include 
#include "atsmsreceive.h"
#include "atsmsstoreread.h"
#include "atsmsack.h"
#include "atmanager.h"
#include "activecommandstore.h"
#include "mslogger.h"
#include "smsatutil.h"

//constant defination
_LIT8(KCMTMatchString,"+CMT:*");
_LIT8(KCMTResponseString,"+CMT:");
_LIT8(KAsterisk,"*");

#ifdef _DEBUG
const TInt KLogMaxCharNumPerLine = 160;
#endif
// ---------------------------------------------------------------------------
// CAtSmsReceive::CAtSmsReceive
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsReceive::CAtSmsReceive(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                             :CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("CAtSmsReceive::CAtSmsReceive called"));
	iAtType = ELtsyAT_Sms_ReceiveMessage;
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::~CAtSmsReceive
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsReceive::~CAtSmsReceive()
	{
	LOGTEXT(_L8("CAtSmsReceive::~CAtSmsReceive called"));
	RemoveAllExpectString();
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsReceive* CAtSmsReceive::NewLC(CGlobalPhonemanager& aGloblePhone, 
                                    CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsReceive* self = new (ELeave)CAtSmsReceive(aGloblePhone, 
                                                    aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtSmsReceive* CAtSmsReceive::NewL(CGlobalPhonemanager& aGloblePhone, 
                                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CAtSmsReceive* self=CAtSmsReceive::NewLC(aGloblePhone, 
                                             aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsReceive::ConstructL()
	{
	CAtCommandBase::ConstructL();
	//Add expecting string
	RemoveAllExpectString();
	AddExpectStringL(KCMTMatchString);
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsReceive::StartRequest()
	{
	iState = EReceiveReady;
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->AddUnsolicitedAtCommand(this);
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsReceive::ExecuteCommand()
	{
	
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsReceive::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
/**
 * Handle the events from the comm port
 *ValidateExpectString
 * @param aSource denotes if event is due to read, write or timeout
 */
	{
	if(aStatus != KErrNone)
		{
		iCtsyDispatcherCallback.CallbackSmsNotifyReceiveSmsMessageInd(iError,EFalse,iMsgReceive);
		return;
		}
	
	if( iState == EReceiveReady )
		{
		LOGTEXT(_L8("CAtSmsReceive::EventSigna,EReadCompletion"));
		//Handle CMT response
		if (iError == KErrNone)
			{
             //want to read the second line
			TRAPD(err,AddExpectStringL(KAsterisk));
			if( err==KErrNone )
				{
				// Wait for the reception of the second line
				iState=EWaitForCmtSecondLine;
				return;
				}
			else
				{
				iError = err;
				}
			}
		iCtsyDispatcherCallback.CallbackSmsNotifyReceiveSmsMessageInd(iError,EFalse,iMsgReceive);
		}
	else if( iState == EWaitForCmtSecondLine )
		{
		LOGTEXT(_L8("CAtSmsReceiveMessage::EventSignal EWaitForCmtSecondLine"));
        RemoveExpectString(KAsterisk);
		iCtsyDispatcherCallback.CallbackSmsNotifyReceiveSmsMessageInd(iError,EFalse,iMsgReceive);
		// We've got the PDU, and will delete the buffer when the line is parsed
		if(iPhoneGlobals.iEventSignalActive == EFalse)
			{
		    ClearBuffer();
			}
		iState = EReceiveReady;
		}
     
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::CMTResponseFirstLineL
// other items were commented in a header
// ---------------------------------------------------------------------------
void  CAtSmsReceive::CMTResponseFirstLineL()
	{
	LOGTEXT(_L8("CAtSmsReceive::CMTResponseFirstLineL Enter funciton"));
	iError = KErrNone;
	TPtrC8 firstLineBuf;
	firstLineBuf.Set(Buffer());
	TInt pos = firstLineBuf.FindF(KCMTResponseString);
	if (pos == KErrNotFound)
		{
		LOGTEXT(_L8("CAtSmsReceive::CMTResponseFirstLineL()\tError - Cannot find '+CMT:' string"));
		iError = KErrNotFound;
		return;
		}
	//skip the string of +CMT:
	pos += KCMTResponseString().Length();
	//skip a  "," character
	while(!(TChar(firstLineBuf[pos]).IsDigit()))
		{
		++pos;
		}
	TInt lenPos = firstLineBuf.Length()-pos;
	TPtrC8 pduLenBuf;
	pduLenBuf.Set(firstLineBuf.Right(lenPos));
	
	
	TLex8 lex(pduLenBuf);
	TUint16 val;
	TInt ret = lex.Val(val,EDecimal);
	if(ret != KErrNone)
		{
		iError = ret;
		return;
		}
	iPduLen = val;
    LOGTEXT2(_L8("New SMS detected of length %d"),iPduLen);
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::CMTResponseSecondLineL
// other items were commented in a header
// ---------------------------------------------------------------------------
void  CAtSmsReceive::CMTResponseSecondLineL() 
	{
	LOGTEXT(_L8("CAtSmsReceive::CMTResponseSecondLineL Enter funciton"));
	iError = KErrNone;
	iAttr.iFlags=0;
	TPtrC8 secondLineBuf;
	secondLineBuf.Set(CurrentLine());
	
#ifdef _DEBUG
	TInt totalLen = secondLineBuf.Length();
	if(totalLen <= KLogMaxCharNumPerLine)
		{
		LOGTEXT2(_L8("CAtSmsReceiveMessage::CMTResponseSecondLineL\tSecondLine=%S"),&secondLineBuf);
		}
	else
		{
		TInt lines = (totalLen/KLogMaxCharNumPerLine);
		TInt remainingStrNum = (totalLen%KLogMaxCharNumPerLine);
		for(TInt i = 0; i < lines; i++)
			{
			TPtrC8 line(secondLineBuf.Mid(i*KLogMaxCharNumPerLine,KLogMaxCharNumPerLine));
			LOGTEXT2(_L8("CAtSmsReceiveMessage::CMTResponseSecondLineL \tSecondLine line=%S"),&line);
			}
		
		TPtrC8 remaining(secondLineBuf.Right(remainingStrNum));
		
		LOGTEXT2(_L8("CAtSmsReceiveMessage::CMTResponseSecondLineL \tSecondLine line=%S"),&remaining);
		}
#endif  
	// Buffer size calculated as follows;
	//   (max pdu size + max prefixed SCA size) * number of ASCII chars used to code an octet
	TBuf8<(RMobileSmsMessaging::KGsmTpduSize+12)*2> localPdu;	
	localPdu.Copy(secondLineBuf);
	//  The CMT frame sometimes contains odd number of nibbles which
	// causes panic in ConvertAsciiToBinary() method. This type of
	// frames will simply be discarded.
	if(localPdu.Length() < (iPduLen*2))
		{
		iError = KErrUnderflow;
		User::Leave(KErrUnderflow);
		}
		
	//
	// Check if we have a prefixed SCA on our pdu.
	// If we do then remove it.
	
	if(localPdu.Length() > (iPduLen*2))
		{
		TInt ret = SmsAtUtil::ReadAndRemoveAddressFromAscii(localPdu,iAttr.iGsmServiceCentre);
		if(ret !=KErrNone)
			{
			LOGTEXT(_L8("CAtSmsReceiveMessage::CMTResponseSecondLineL Failed to read and remove SCA from PDU"));
			iError = ret;
			return;
			}
		else
			{
			iMsgReceive.iServiceCentre = iAttr.iGsmServiceCentre.iTelNumber;    //for iServiceCentre
			iMsgReceive.iMobileScNPI = iAttr.iGsmServiceCentre.iNumberPlan;
			iMsgReceive.iMobileScTON = iAttr.iGsmServiceCentre.iTypeOfNumber;
			iAttr.iFlags|= RMobileSmsMessaging::KGsmServiceCentre;
			}
		}
	//
	// Convert received pdu from ASCII into binary
	TInt ret = SmsAtUtil::ConvertAsciiToBinary(localPdu,iPdu);
	if (ret!=KErrNone)
		{
		LOGTEXT(_L8("CAtSmsReceiveMessage::CMTResponseSecondLineL Failed to code PDU into binary"));
		iError = ret;
		return;
		}
	else
		{
		iAttr.iDataFormat=RMobileSmsMessaging::EFormatGsmTpdu;
		iAttr.iFlags|=RMobileSmsMessaging::KSmsDataFormat;
		iAttr.iStatus = RMobileSmsMessaging::EMtMessageUnstoredClientAck;
		iAttr.iFlags|=RMobileSmsMessaging::KIncomingStatus;
		}

	LOGTEXT2(_L8("Received PDU String is %S"),&iPdu);
	iMsgReceive.iSmsMsg = iPdu;
	}

// ---------------------------------------------------------------------------
// CAtSmsReceive::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtSmsReceive::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if(iState == EReceiveReady)
		{
		CMTResponseFirstLineL();
		}
	else if(iState == EWaitForCmtSecondLine)
		{
		CMTResponseSecondLineL();
		}
	}

//End of file
