// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Writer.h"
#include "rpsasciirqstdefs.h"
#include "sender.h"
#include "rpsloggermacros.h"


CWriter* CWriter::NewL()
	{
	CWriter* writer = new(ELeave)CWriter();
	CleanupStack::PushL(writer);
	writer->ConstructL();
	CleanupStack::Pop();
	return writer;
	}
	
CWriter::~CWriter()
	{
	delete iSender;
	}
	
void CWriter::ConstructL()
	{
	iMsgNumCntr = 1;
	iSender = CSender::NewL();
	}
	
//
// IssueRequestL. This version is not interested in the response data.
//
// aOpCode - request's opcode (in)
// aArgArray - request's argument array (in)
//
void CWriter::IssueRequestL(const TDesC8& aOpCode, CDesCArrayFlat& aArgArray)
	{
	CRpsMsg* dummyRpsMsg = CRpsMsg::NewLC();
	
	IssueRequestL(aOpCode, aArgArray, *dummyRpsMsg);
	
	CleanupStack::PopAndDestroy(dummyRpsMsg);  // Not interested in response
	}
	
//
// IssueRequestL. This version returns the response data.
//  
// aOpCode - request's opcode (in)
// aArgArray - request's argument array (in)
// aRespArray - response data received from Slave (out)
//
void CWriter::IssueRequestL(const TDesC8& aOpCode, CDesCArrayFlat& aArgArray, CRpsMsg& aRespMsg)
	{
	// Formulate necessary msg data from the OpCode and arguments into a descriptor array
	// and pass to CSender
	
	CRpsMsg* rpsMsg = CRpsMsg::NewLC(KMsgTypeRqst, aOpCode, NextMsgNum(), aArgArray);

	HBufC8* response = 0;

	TInt ret = iSender->SendRequestL(*rpsMsg, &response);
	CleanupStack::PopAndDestroy(rpsMsg);
	
	CleanupStack::PushL(response);
	
	if(ret != KErrNone)
		{
		// TODO - log error
		// 
		// A ret error indicates a comms type problem on sending the reqst or receiving a response
		RDEBUGPRINTLOGGER2(_L("RPS: CWriter::IssueRequestL() A leaving err = %d"), ret);
		User::LeaveIfError(ret);
		}
	
	// Populate reply msg with the response received from RPS slave
	ret = aRespMsg.InternalizeL(*response);			

	if(ret != KErrNone)
		{
		// TODO - log error
		User::LeaveIfError(ret);
		}
	
	// Do some parsing of the response received
	if(aRespMsg.ArraySize() < KRespMsgElementsNoArgs)
		{
		// TODO - log error
		RDEBUGPRINTLOGGER1(_L("RPS: CWriter::IssueRequestL() B bad no. of args"));
		User::Leave(KErrCorrupt);
		}
	
	TPtrC8 msgType = aRespMsg.GetMsgType();
	if(msgType.Compare(KMsgTypeResp))
		{
		RDEBUGPRINTLOGGER1(_L("RPS: CWriter::IssueRequestL() C unknown rsp"));
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PopAndDestroy();	// response
	}
	
TUint CWriter::NextMsgNum()
	{
	
	if(iMsgNumCntr == 99)
		iMsgNumCntr = 01;
	TUint ret(iMsgNumCntr);
	++iMsgNumCntr;
	return ret;
	}
	

//---------------------------------------------------------------------------------------
// CToAscii methods
//---------------------------------------------------------------------------------------

const TDesC8& CToAscii::CallNumber()
	{
	return KCallNumber;
	}
	
const TDesC8& CToAscii::AnswerNextCall()
	{
	return KAnswerNextCall;
	}
	
const TDesC8& CToAscii::RejectNextCall()
	{
	return KRejectNextCall;
	}
	
const TDesC8& CToAscii::IgnoreNextCall()
	{
	return KIgnoreNextCall;
	}
	
const TDesC8& CToAscii::HangUp()
	{
	return KHangUp;
	}
	
const TDesC8& CToAscii::Hold()
	{
	return KHold;
	}
	
const TDesC8& CToAscii::Resume()
	{
	return KResume;
	}
	
const TDesC8& CToAscii::SendDataBackOnDataPort()
	{
	return KSendDataBackOnDataPort;
	}
		
const TDesC8& CToAscii::ReqstSlaveTelNumbers()
	{
	return KHShakeReqstSlaveTelNumbers;
	}
	
const TDesC8& CToAscii::CallNumberHangUp()
	{
	return KDialHangUp;
	}
	
const TDesC8& CToAscii::CallNumberHangUpBeforeConnect()
	{
	return KDialHangUpBeforeConnect;
	}
	
const TDesC8& CToAscii::CallNumberHangUpAfterConnect()
	{
	return KDialHangUpAfterConnect;
	}
	
const TDesC8& CToAscii::AnswerNextCallHangUp()
	{
	return KAnswerNextCallHangUp;
	}
	
const TDesC8& CToAscii::AnswerNextCallHold()
	{
	return KAnswerNextCallHold;
	}
	
const TDesC8& CToAscii::HangUpNextCallBeforeConnect()
	{
	return KHangUpNextCallBeforeConnect;
	}

const TDesC8& CToAscii::Cleanup()
	{
	return KCleanup;
	}
		
const TDesC8& CToAscii::CheckRpsPresent()
	{
	return KCheckRpsPresent;
	}
