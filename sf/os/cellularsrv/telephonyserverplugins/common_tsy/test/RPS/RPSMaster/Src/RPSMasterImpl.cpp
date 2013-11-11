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

#include "RPSMasterImpl.h"
#include "rpsasciirqstdefs.h"
#include "rpsloggermacros.h"
#include "rpscommon.h"
#include "Writer.h"

const TInt KCharWidth = 4;
const TInt KMaxDelaySecs = 7200;

CRPSMasterImpl* CRPSMasterImpl::NewL()
	{
	CRPSMasterImpl* impl = new (ELeave)CRPSMasterImpl();
	CleanupStack::PushL(impl);
	impl->ConstructL();
	CleanupStack::Pop();
	return impl;
	}

CRPSMasterImpl::~CRPSMasterImpl()
 	{
 	delete iWriter;
 	}
 	
void CRPSMasterImpl::ConstructL()
	{
	TRAPD(err, iWriter = CWriter::NewL());
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::ConstructL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	// Issue our own reqst to Slave for exchanging any required data
	HandShakeL();
	}
	
void CRPSMasterImpl::HandShakeL()
	{
	ReqstSlaveTelNumbersL();
	}
	
void CRPSMasterImpl::ReqstSlaveTelNumbersL()  
	{
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	// Response msg
	CRpsMsg* respMsg = CRpsMsg::NewLC();
	
	// If error, log to testbase logger then Leave
	TRAPD(err, iWriter->IssueRequestL(CToAscii::ReqstSlaveTelNumbers(), *array, *respMsg));
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	else
		{
		// Extract response arguments
		// According to RPSAsciiRqstDefs.h the order is: Voice, Data, Fax
		TPtrC8 ptr1 = respMsg->GetArgL(1);
		iSlaveTelNumber.Copy(ptr1);
		
		TPtrC8 ptr2 = respMsg->GetArgL(2);
		iSlaveTelNumberData.Copy(ptr2);
		
		TPtrC8 ptr3 = respMsg->GetArgL(3);
		iSlaveTelNumberFax.Copy(ptr3);
		}

	CleanupStack::PopAndDestroy(respMsg);
	CleanupStack::PopAndDestroy(array);
	}
	
void CRPSMasterImpl::doCallNumberL(const TDesC& aNumToDial, TEtelLine aLine)
	{
	// Validate arguments
	if(aNumToDial.Length() == 0)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doCallNumberL() A leaving err = %d"), KErrArgument);
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
		
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
		
	array->AppendL(aNumToDial);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::CallNumber(), *array));	
	
	
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::CallNumberL() B leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}
	
	
void CRPSMasterImpl::doAnswerNextCallL(TEtelLine aLine)
	{
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::AnswerNextCall(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doAnswerNextCallL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);	
	}
	
void CRPSMasterImpl::doRejectNextCallL(TEtelLine /*aLine*/)
	{
	
	}
	
void CRPSMasterImpl::doIgnoreNextCallL(TEtelLine /*aLine*/)
	{
	
	}
	
void CRPSMasterImpl::doHangUpL(TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth);  
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::HangUp(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doHangUpL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}
	
void CRPSMasterImpl::doHoldL(TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth); 
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::Hold(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doHoldL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}

	
void CRPSMasterImpl::doResumeL(TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth);  
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::Resume(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doResumeL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}


void CRPSMasterImpl::doSendDataBackOnDataPortL(const TDesC& aDataToSend)
	{
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
 
	array->AppendL(aDataToSend);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::SendDataBackOnDataPort(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doSendDataBackOnDataPortL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}
	
void CRPSMasterImpl::doCallNumberHangUpL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aNumToDial.Length() == 0 || aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}

	array->AppendL(aNumToDial);
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth); 
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::CallNumberHangUp(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doCallNumberHangUpL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}
	

// This requests the RPS to call us, then hangup the call n seconds after the call starts to ring.
// There is no control over whether the call gets answered by the network before the delay completes. 
void CRPSMasterImpl::doCallNumberHangUpBeforeConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aNumToDial.Length() == 0 || aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
		switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}

	array->AppendL(aNumToDial);
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth); 
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::CallNumberHangUpBeforeConnect(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doCallNumberHangUpBeforeConnectL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);	
	}
	
	
// This requests the RPS to call us, then hangup the call n seconds after the call connects.
void CRPSMasterImpl::doCallNumberHangUpAfterConnectL(const TDesC& aNumToDial, TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aNumToDial.Length() == 0 || aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}

	array->AppendL(aNumToDial);
	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth);
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::CallNumberHangUpAfterConnect(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doCallNumberHangUpAfterConnectL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);	
	}
	
	
// This requests the RPS to answer the next call, then hangup the call n seconds after the call connects.
// This method does not make a call to the RPS, but one is required before the next RPS request is made.
void CRPSMasterImpl::doAnswerNextCallHangUpL(TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}

	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth);
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::AnswerNextCallHangUp(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doAnswerNextCallHangUpL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}
	

// This requests the RPS to answer the next call, then put the call on hold n seconds after the call connects.
// This method does not make a call to the RPS, but one is required before the next RPS request is made.
void CRPSMasterImpl::doAnswerNextCallHoldL(TEtelLine aLine, TInt aDelayInSec)
	{
	// Validate arguments
	if(aDelayInSec < 0)
		{
		User::Leave(KErrArgument);
		}
		
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}

	
	if(aDelayInSec < 0) aDelayInSec = 0;
	if(aDelayInSec > 7200) aDelayInSec = KMaxDelaySecs;  // Impose delay limit of 2 hours max
	TBuf<4> delayString;
	delayString.NumFixedWidth(aDelayInSec, EDecimal, KCharWidth);
	array->AppendL(delayString);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::AnswerNextCallHold(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doAnswerNextCallHoldL() leaving err = %d"), err);
		User::Leave(err);
		}
		
	CleanupStack::PopAndDestroy(array);
	}

// This requests the RPS to hangup the incoming call without answering it
void CRPSMasterImpl::doHangUpNextCallBeforeConnectL(TEtelLine aLine)
	{
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	switch(aLine)
		{
	case EVoiceLine:
		array->AppendL(KRpsVoiceLine);
		break;
	case EDataLine:
		array->AppendL(KRpsDataLine);
		break;
	case EFaxLine:
		array->AppendL(KRpsFaxLine);
		break;		
	default:
		User::Leave(KErrArgument);
		}
		
	TRAPD(err, iWriter->IssueRequestL(CToAscii::HangUpNextCallBeforeConnect(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doHangUpNextCallBeforeConnectL() leaving err = %d"), err);
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(array);
	}
	
void CRPSMasterImpl::doCleanupL()
	{
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);

	// don't care if it fail
	TRAP_IGNORE(iWriter->IssueRequestL(CToAscii::Cleanup(), *array));

	CleanupStack::PopAndDestroy(array);		
	}
		
void CRPSMasterImpl::doCheckRpsPresentL(TBool& aPresent)
	{
	aPresent = EFalse;
	
	// Create argument array
	CDesCArrayFlat* array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(array);
	
	TRAPD(err, iWriter->IssueRequestL(CToAscii::CheckRpsPresent(), *array));
	if(err != KErrNone)
		{
		RDEBUGPRINTLOGGER2(_L("RPS: CRPSMasterImpl::doCheckRpsPresentL() leaving err = %d"), err);
		User::Leave(err);
		}
	else
		{
		aPresent = ETrue;
		}
		
	CleanupStack::PopAndDestroy(array);	
	}
	

