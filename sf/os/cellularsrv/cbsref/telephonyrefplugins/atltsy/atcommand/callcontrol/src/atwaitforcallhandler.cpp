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
// Wait for 2nd incoming call

//system include

//user include
#include "atwaitforcallhandler.h"
#include "atclcc.h"
#include "atring.h"
#include "atmanager.h"
#include "activecommandstore.h"
#include "ltsycallinformationmanager.h"
#include "mslogger.h"

CATWaitForCallHandler* CATWaitForCallHandler::NewL(CGlobalPhonemanager& aGloblePhone,
		                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::NewL()"));
	
	CATWaitForCallHandler* self = CATWaitForCallHandler::NewLC(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

CATWaitForCallHandler* CATWaitForCallHandler::NewLC(CGlobalPhonemanager& aGloblePhone,
		                                            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::NewLC()"));
	
	CATWaitForCallHandler* self = new (ELeave) CATWaitForCallHandler(aGloblePhone, aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CATWaitForCallHandler::~CATWaitForCallHandler()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::~CATWaitForCallHandler()"));
	
	delete iAtRing;
	iAtRing = NULL;
	
	delete iAtClcc;
	iAtClcc = NULL;
	}


CATWaitForCallHandler::CATWaitForCallHandler(CGlobalPhonemanager& aGloblePhone,
		                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
											 :CATCommandControlBase(aGloblePhone, aCtsyDispatcherCallback) 
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::CATWaitForCallHandler()"));
	
	iPhoneGlobals.GetCallInfoManager().SetIncomingCallFlag(EFalse);
	}

void CATWaitForCallHandler::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::ConstructL()"));
	
	iAtClcc = CATClcc::NewL(iPhoneGlobals, iCtsyDispatcherCallback);
	iAtRing = CATRing::NewL(iPhoneGlobals, iCtsyDispatcherCallback);
	iAtRing->SetAtCommandObserver(this);
	}


void CATWaitForCallHandler::StartToWaitForCall()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::StartToWaitForCall()"));
	
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->AddUnsolicitedAtCommand(iAtRing);
	}

TInt CATWaitForCallHandler::ProcessIncomingCallInfomation()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::ProcessIncomingCallInfomation()"));
	
	TInt nCallId = iPhoneGlobals.GetCallInfoManager().FindIncomingCallId();
	
	if (KErrNotFound == nCallId)
		{
		return KErrNotFound;
		}
	
	//Incoming call information
	TLtsyCallInformation& tIncomingCallInfo(iPhoneGlobals.GetCallInfoManager().GetCallInformationByCallId(nCallId));
	
	RMobileCall::TMobileCallInfoV1 tMobileCallInfo;
	
	//Call id
	tMobileCallInfo.iCallId = tIncomingCallInfo.GetCallId();
	
	//Call service
	TLtsyCallInformation::TCallMode tCallService(tIncomingCallInfo.GetCallMode());
	if (TLtsyCallInformation::EVoiceCall == tCallService)
		{
		tMobileCallInfo.iService = RMobilePhone::EVoiceService;
		}
	else if (TLtsyCallInformation::EDataCall == tCallService)
		{
		tMobileCallInfo.iService = RMobilePhone::ECircuitDataService;
		}
	else if (TLtsyCallInformation::EFaxCall == tCallService)
		{
		tMobileCallInfo.iService = RMobilePhone::EFaxService;
		}
	else
		{
		tMobileCallInfo.iService = RMobilePhone::EServiceUnspecified;
		}
		
	//Call back
	iCtsyDispatcherCallback.CallbackCallControlNotifyIncomingCallInd(KErrNone, tMobileCallInfo);
	
	//Ring State
	tMobileCallInfo.iStatus = RMobileCall::EStatusRinging;
	
	//Telephone number
	tMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(tIncomingCallInfo.GetMobileTelNum());
	
	iCtsyDispatcherCallback.CallbackCallControlNotifyCallInfoChangeInd(KErrNone, tMobileCallInfo);
	
	return KErrNone;
	}

void CATWaitForCallHandler::AtCommandExecuteComplete(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CATWaitForCallHandler::AtCommandExecuteComplete()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aEventSource = %d\taStatus = %d"),aStatus);
		
	//Process read or timeout error
	if (aStatus != KErrNone)
		{
		//Reset Ring Count
		iPhoneGlobals.GetCallInfoManager().SetIncomingCallFlag(EFalse);
		
		//Init ATCLCC Variable
		iAtClcc->InitVariable();
		
		//Init InComing Call State
		TInt nCall = iPhoneGlobals.GetCallInfoManager().FindIncomingCallId();
		if (nCall != KErrNotFound)
			{
			iPhoneGlobals.GetCallInfoManager().ResetCallInformationByCallId(nCall);
			}
		return;
		}
	
	//Clear Ring String
	iAtRing->ClearCurrentLine();
	
	//CLCC find incoming information If first ring  coming
	if (!iPhoneGlobals.GetCallInfoManager().GetIncomingCallFlag())
		{
		TInt nRet = iPhoneGlobals.CheckGlobalPhoneStatus();
		if (nRet == KErrNone)
			{
			iPhoneGlobals.iEventSignalActive = ETrue;
			iPhoneGlobals.GetCallInfoManager().SetIncomingCallFlag(ETrue);
			iAtClcc->StartRequest();			
			}
		}	
	
	//Find At Clcc OK
	if (iAtClcc->GetProcessStates() == CATClcc::EProcessOK)
		{
		TInt nRes = ProcessIncomingCallInfomation();
		iAtClcc->InitVariable();
		}	
	
	//Process AT CLCC error
	if (iAtClcc->GetProcessStates() == CATClcc::EProcessError )
		{
		iAtClcc->InitVariable();
		}
	}

//End of file
