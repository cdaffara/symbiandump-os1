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
// @file atclcc.cpp
// This contains CATClcc which query incoming or waitting call information.
// 

//system include

//user include
#include "mslogger.h"
#include "atmanager.h"
#include "activecommandstore.h"
#include "atclcc.h"
#include "callinformationquerycompletenotify.h"
#include "ltsycommondefine.h"
#include "ltsycallinformationmanager.h"

//const define
const TInt KLtsyClccReadTimeout = 2;

//List current call information 
_LIT8(KLtsyCLCCCommand, "AT+CLCC\r");
_LIT8(KLtsyCLCCMatchString, "+CLCC:*");


// ---------------------------------------------------------------------------
// CATClcc::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATClcc* CATClcc::NewL(CGlobalPhonemanager& aGloblePhone,
			           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::NewL()"));
	
	CATClcc* self = CATClcc::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CATClcc::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CATClcc* CATClcc::NewLC(CGlobalPhonemanager& aGloblePhone,
		                CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::NewLC()"));
	
	CATClcc* self = new (ELeave) CATClcc(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


// ---------------------------------------------------------------------------
// CATClcc::~CATClcc
// other items were commented in a header
// ---------------------------------------------------------------------------
CATClcc::~CATClcc()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::~CATClcc()"));
	}

// ---------------------------------------------------------------------------
// CATClcc::CATClcc
// other items were commented in a header
// ---------------------------------------------------------------------------
CATClcc::CATClcc(CGlobalPhonemanager& aGloblePhone, 
		         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
				 :CAtCommandBase(aGloblePhone, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::CATClcc()"));
	
	iAtType = ELtsyAT_General_CLCC;
	
	iQueryCallInfoNotify = NULL;
	InitVariable();
	}

// ---------------------------------------------------------------------------
// CATClcc::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::ConstructL()"));
	
	//Invoke base class function
	CAtCommandBase::ConstructL();
	
	//Set read and write timeout
	SetTimeOut(KLtsyDefaultWriteTimeOut, KLtsyClccReadTimeout);
	
	//Add expecting string
	AddExpectStringL(KLtsyCLCCMatchString);
	}

// ---------------------------------------------------------------------------
// CATClcc::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::ExecuteCommand()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::ExecuteCommand()"));
	
	iOKFounded = EFalse;
	iTxBuffer.Copy(KLtsyCLCCCommand);
	Write();
	iATStep = EATWaitForWriteComplete;
	iProcessStates = EProcessInProgress;
	}

// ---------------------------------------------------------------------------
// CATClcc::InitVariable
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::InitVariable()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::InitVariable()"));
	
	iOKFounded = EFalse;
	iIOStatus = KErrNone;
	iATResult = KErrNone;
	iATStep = EATNotInProgress;
	iProcessStates = EProcessNotInProgress;	
	}

// ---------------------------------------------------------------------------
// CATClcc::AddQueryCallInformationCompleteNotify
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::AddQueryCallInformationCompleteNotify(MCallInformationQueryCompleteNotify* aQueryCallInfoNotify)
	{
	iQueryCallInfoNotify = aQueryCallInfoNotify;
	}

// ---------------------------------------------------------------------------
// CATClcc::GetProcessStates
// other items were commented in a header
// ---------------------------------------------------------------------------
CATClcc::TATProcessStates CATClcc::GetProcessStates() const
	{
	return iProcessStates;
	}

// ---------------------------------------------------------------------------
// CATClcc::SetProcessStates
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::SetProcessStates(CATClcc::TATProcessStates aProcessState)
	{
	iProcessStates = aProcessState;
	}

// ---------------------------------------------------------------------------
// CATClcc::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::StartRequest()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::StartRequest()"));
	
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATClcc::ParseCLCCResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::ParseCLCCResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::ParseCLCCResponseL()"));
	
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array, aResponseBuf);
	TInt nCount = array.Count();
	
	//  1       2      3      4       5       6
	//+CLCC : <id1>, <dir>, <stat>, <mode>, <mpty>
	//Its count must >= 6
	if (nCount < 6)	  
		{
		CleanupStack::PopAndDestroy(1);
		return;
		}
	
	//Parse +CLCC :* buffer
	//Only care Waiting call and incoming call
	TInt nVal(0);
	TLex8 tLex(array[3]);
	if (tLex.Val(nVal) == KErrNone)
		{		
		if (nVal == TLtsyCallInformation::EActiveCall ||
			nVal == TLtsyCallInformation::EHeldCall ||
			nVal == TLtsyCallInformation::EDialingCall ||
			nVal == TLtsyCallInformation::EAlertingCall )
			{
			CleanupStack::PopAndDestroy(1);
			LOGTEXT(_L8("[Ltsy CallControl] First Parse state of the call not find incoming call and waiting call"));
			return;			
			}
		
		}
	else
		{
		CleanupStack::PopAndDestroy(1);
		LOGTEXT(_L8("[Ltsy CallControl] First Parse state of the call Failing"));
		return;
		}
	
	//Parse incoming call or waiting call
	
	//Call id
	tLex.Assign(array[1]);
	if ((tLex.Val(nVal) != KErrNone)||
	    (nVal < KLtsyMinCallId)	||
		(nVal > KLtsyMaxCallId))
		{
		CleanupStack::PopAndDestroy(1);
		LOGTEXT(_L8("[Ltsy CallControl] Parse call id of the call Failing"));
		return;
		}
   
	//Parse call id success
	TLtsyCallInformation& callinfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(nVal));
	callinfo.SetCallIdIsUsedInfo(TLtsyCallInformation::EUsed);
	callinfo.SetCallId(nVal);
	
	TBool bSuccess(ETrue);
	for (TInt n = 2; bSuccess && n < nCount; n++)
		{
		//Direction Call
		if (n == 2)
			{
			tLex.Assign(array[n]);
			if (tLex.Val(nVal) == KErrNone)
				{
				if (TLtsyCallInformation::EMoCall == nVal)
					{
					callinfo.SetCallDirection(TLtsyCallInformation::EMoCall);
					}
				else
					{
					callinfo.SetCallDirection(TLtsyCallInformation::EMtCall);
					}
				}
			else
				{
				bSuccess = EFalse;
				LOGTEXT(_L8("[Ltsy CallControl] Parse direction call Failing"));
				}
			}
		
		//State of the Call
		if (n == 3)
			{
			tLex.Assign(array[n]);
			if (tLex.Val(nVal) == KErrNone)
				{
				if (TLtsyCallInformation::EInComingCall == nVal)
					{
					callinfo.SetCallState(TLtsyCallInformation::EInComingCall);
					}
				else
					{
					callinfo.SetCallState(TLtsyCallInformation::EWaitingCall);
					}
				}
			else
				{
				bSuccess = EFalse;
				LOGTEXT(_L8("[Ltsy CallControl] Parse state of the call Failing"));				
				}
			}
		
		//Teleservice
		if (n == 4)
			{
			tLex.Assign(array[n]);
			if (tLex.Val(nVal) == KErrNone)
				{
				if (TLtsyCallInformation::EVoiceCall == nVal)
					{
					callinfo.SetCallMode(TLtsyCallInformation::EVoiceCall);
					}
				else if (TLtsyCallInformation::EDataCall == nVal)
					{
					callinfo.SetCallMode(TLtsyCallInformation::EDataCall);
					}
				else if (TLtsyCallInformation::EFaxCall == nVal)
					{
					callinfo.SetCallMode(TLtsyCallInformation::EFaxCall);
					}
				else
					{
					callinfo.SetCallMode(TLtsyCallInformation::EUnKnowCall);
					}
				}
			else
				{
				bSuccess = EFalse;
				LOGTEXT(_L8("[Ltsy CallControl] Parse teleservice Failing"));					
				}
			}

		//multiparty
		if (n == 5)
			{
			tLex.Assign(array[n]);
			if (tLex.Val(nVal) == KErrNone)
				{
				if (TLtsyCallInformation::ENotConference == nVal)
					{
					callinfo.SetConferenceCall(TLtsyCallInformation::ENotConference);
					}
				else
					{
					callinfo.SetConferenceCall(TLtsyCallInformation::EIsConference);
					}
				}
			else
				{
				bSuccess = EFalse;
				LOGTEXT(_L8("[Ltsy CallControl] Parse multiparty Failing"));				
				}
			}
		
		//telenum
		if (n == 6)
			{
			callinfo.SetMobileTelNum(array[n]);
			}		

		//type of address
		if (n == 7)
			{
			tLex.Assign(array[n]);
			if (tLex.Val(nVal) == KErrNone)
				{
				callinfo.SetAddressType(nVal);
				}
			else
				{
				bSuccess = EFalse;
				LOGTEXT(_L8("[Ltsy CallControl] Parse type of address Failing"));				
				}
			}
		
		//alpah
		if (n == 8)
			{
			callinfo.SetMobileName(array[n]);
			}
		}
	
	//Parse failer
	if (!bSuccess)
		{
		iPhoneGlobals.GetCallInfoManager().ResetCallInformationByCallId(callinfo.GetCallId());
		}
	
	CleanupStack::PopAndDestroy(1);
	}

// ---------------------------------------------------------------------------
// CATClcc::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::ParseResponseL(const TDesC8& aResponseBuf)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::ParseResponseL()"));
	
	//Process expecting string +CLCC : *
	if (aResponseBuf.Match(KLtsyCLCCMatchString) == 0)
		{
		ParseCLCCResponseL(aResponseBuf);
		}
	else if (aResponseBuf.Match(KLtsyOkString) == 0) //Process expecting string OK
		{
		iATResult = KErrNone;
		iProcessStates = EProcessOK;
		iOKFounded = ETrue;		
		}
	else if (aResponseBuf.Match(KLtsyErrorString) == 0) //Process expecting string ERROR
		{
		iATResult = KErrArgument;
		iProcessStates = EProcessError;
		iOKFounded = EFalse;		
		}
	}

// ---------------------------------------------------------------------------
// CATClcc::HandleIOError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::HandleIOError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::HandleIOError()"));
	
	iProcessStates = EProcessError;
	
	if (iQueryCallInfoNotify != NULL)
		{
		iQueryCallInfoNotify->QueryCallInformationComplete(iIOStatus);
		}
	}

// ---------------------------------------------------------------------------
// CATClcc::HandleResponseError
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::HandleResponseError()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::HandleResponseError()"));
	
	if (iQueryCallInfoNotify != NULL)
		{
		iQueryCallInfoNotify->QueryCallInformationComplete(iATResult);
		}	
	}

// ---------------------------------------------------------------------------
// CATClcc::HandleCLCCComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::HandleCLCCComplete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::HandleCLCCComplete()"));
	
	if (iQueryCallInfoNotify != NULL)
		{
		iQueryCallInfoNotify->QueryCallInformationComplete(KErrNone);
		}
	}

// ---------------------------------------------------------------------------
// CATClcc::Complete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::Complete()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::Complete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] iIOStatus = %d"), iIOStatus);
	LOGTEXT2(_L8("[Ltsy CallControl] iATResult = %d"), iATResult);
	
	//Remove Ative Command and stop timer
	CAtCommandBase::Complete();
	
	//Let other command can use I/O port
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// ---------------------------------------------------------------------------
// CATClcc::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATClcc::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{  
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATClcc::EventSignal()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aEventSource = %d\taStatus = %d"),aStatus);
	
	iIOStatus = aStatus;
	if (iIOStatus != KErrNone)
		{
		HandleIOError();	
		Complete();
		return;
		}
	
	switch (iATStep)
		{
		case EATWaitForWriteComplete:
			iATStep = EATReadComplete;
			break;
			
		case EATReadComplete:
			ClearCurrentLine();
			if (iATResult == KErrNone)
				{
				if (iOKFounded)
					{
					HandleCLCCComplete();
					Complete();
					}
				}
			else
				{
				HandleResponseError();
				Complete();
				}
			break;
			
		default:
			break;
		}
	}

//End of file
