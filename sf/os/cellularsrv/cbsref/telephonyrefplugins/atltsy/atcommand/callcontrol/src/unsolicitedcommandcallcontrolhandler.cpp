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
// CUnsolicitedCommandCallControlHandler. The handler for handling the unsolicited command related to call control

//system include

//user include
#include "unsolicitedcommandcallcontrolhandler.h"
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include "ltsycommondefine.h"
#include "globalphonemanager.h"
#include "unsolicitedparams.h"
#include "atclcc.h"
#include "ltsycallinformationmanager.h"
#include "mslogger.h"

CUnsolicitedCommandCallControlHandler* CUnsolicitedCommandCallControlHandler::NewL(CGlobalPhonemanager& aGloblePhone,
		                                                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::NewL()"));
	
	CUnsolicitedCommandCallControlHandler* self = CUnsolicitedCommandCallControlHandler::NewLC(aGloblePhone, 
			                                                                                   aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

CUnsolicitedCommandCallControlHandler* CUnsolicitedCommandCallControlHandler::NewLC(CGlobalPhonemanager& aGloblePhone,
		                                                                            CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::NewLC()"));
	
	CUnsolicitedCommandCallControlHandler* self = new (ELeave) CUnsolicitedCommandCallControlHandler(aGloblePhone, 
			                                                                                         aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUnsolicitedCommandCallControlHandler::~CUnsolicitedCommandCallControlHandler()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::~CUnsolicitedCommandCallControlHandler()"));
	}

CUnsolicitedCommandCallControlHandler::CUnsolicitedCommandCallControlHandler(CGlobalPhonemanager& aGloblePhone,
		                                                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
																			 :iGlobalPhone(aGloblePhone),
																			  iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::CUnsolicitedCommandCallControlHandler()"));
	}

void CUnsolicitedCommandCallControlHandler::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::ConstructL()"));
	
	iAtClcc = CATClcc::NewL(iGlobalPhone, iCtsyDispatcherCallback);
	
	iAtClcc->AddQueryCallInformationCompleteNotify(this);
	}

void CUnsolicitedCommandCallControlHandler::QueryCallInformationComplete(TInt /*aError*/)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::QueryCallInformationComplete()"));
	
	if (iAtClcc->GetProcessStates() == CATClcc::EProcessOK)
		{
		TInt nCallId = iGlobalPhone.GetCallInfoManager().FindWaitingCallId();
		
		LOGTEXT2(_L8("[Ltsy CallControl] The Waiting Call Id = %d"), nCallId);
		if (KErrNotFound == nCallId)
			{
			iAtClcc->InitVariable();
			return;
			}
		
		//Waiting call information
		TLtsyCallInformation& tWaitingCallInfo(iGlobalPhone.GetCallInfoManager().GetCallInformationByCallId(nCallId));
		
		RMobileCall::TMobileCallInfoV1 tMobileCallInfo;
		
		//Call id
		tMobileCallInfo.iCallId = tWaitingCallInfo.GetCallId();
		
		//Call service
		TLtsyCallInformation::TCallMode tCallService(tWaitingCallInfo.GetCallMode());
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
		
		//Waiting States
		tMobileCallInfo.iStatus = RMobileCall::EStatusRinging;		
		
		//Telephone number
		tMobileCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Copy(tWaitingCallInfo.GetMobileTelNum());
			
		//Callback
		iCtsyDispatcherCallback.CallbackCallControlNotifyCallInfoChangeInd(KErrNone, tMobileCallInfo);		
		}
	
	//Init iAtCll for reusing it
	iAtClcc->InitVariable();	
	}

void CUnsolicitedCommandCallControlHandler::HandleSimRemoveEvent()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::HandleSimRemoveEvent()"));
	
	TInt nErr = KErrEtelNoCarrier;
	
	for (TInt nCallId = KLtsyMinCallId; nCallId <= KLtsyMaxCallId; nCallId++)
		{
		TLtsyCallInformation& tCallInfo(iGlobalPhone.GetCallInfoManager().GetCallInformationByCallId(nCallId));
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo())
			{
			//Notify disconnecting status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusDisconnecting);				
			
			//Nofity idel status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusIdle);				
			}
		}
	
	iGlobalPhone.GetCallInfoManager().ResetAllCalInformation();	
	}

void CUnsolicitedCommandCallControlHandler::HandleProductInitEvent()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::HandleProductInitEvent()"));
	
	TInt nErr = KErrEtelNoCarrier;
	
	for (TInt nCallId = KLtsyMinCallId; nCallId <= KLtsyMaxCallId; nCallId++)
		{
		TLtsyCallInformation& tCallInfo(iGlobalPhone.GetCallInfoManager().GetCallInformationByCallId(nCallId));
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo())
			{
			//Notify disconnecting status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusDisconnecting);				
			//Notify idel status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusIdle);				
			}
		}
	
	iGlobalPhone.GetCallInfoManager().ResetAllCalInformation();		
	}

void CUnsolicitedCommandCallControlHandler::HandleNetworkLostEvent()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::HandleNetworkLostEvent()"));
	
	TInt nErr = KErrEtelNoCarrier;	
	
	for (TInt nCallId = KLtsyMinCallId; nCallId <= KLtsyMaxCallId; nCallId++)
		{
		TLtsyCallInformation& tCallInfo(iGlobalPhone.GetCallInfoManager().GetCallInformationByCallId(nCallId));
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo())
			{
			//Notify disconnecting status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusDisconnecting);				
			//Notify idel status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(nErr,
					                                                             nCallId, 
					                                                             RMobileCall::EStatusIdle);				
			}
		}
	
	iGlobalPhone.GetCallInfoManager().ResetAllCalInformation();		
	}

void CUnsolicitedCommandCallControlHandler::HandleCallCreatedEvent(const TDesC8& aWindPkg)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::HandleCallCreatedEvent()"));
	
	TUnsolicitedParams::TUnsolicitedParamsPckgBuf* pPckg = (TUnsolicitedParams::TUnsolicitedParamsPckgBuf*)(&aWindPkg);
	TInt nCallId = (*pPckg)().iIdx;
	LOGTEXT2(_L8("[Ltsy CallControl] nCallId = %d"), nCallId);
	
	if (iAtClcc->GetProcessStates() == CATClcc::EProcessNotInProgress)
		{
		TInt nRet = iGlobalPhone.CheckGlobalPhoneStatus();
		if (nRet == KErrNone)
			{
			iGlobalPhone.iEventSignalActive = ETrue;
			iAtClcc->StartRequest();			
			}
		}
	}

void CUnsolicitedCommandCallControlHandler::HandleCallReleasedEvent(const TDesC8& aWindPkg)
	{
	//if call id is incoming call reset incoming call states
	iGlobalPhone.GetCallInfoManager().SetIncomingCallFlag(EFalse);
	
	//Notify CTSY incoming call release
	TUnsolicitedParams::TUnsolicitedParamsPckgBuf* pPckg = (TUnsolicitedParams::TUnsolicitedParamsPckgBuf*)(&aWindPkg);
	TInt nCallId = (*pPckg)().iIdx;		
	LOGTEXT2(_L8("[Ltsy CallControl] nCallId = %d"), nCallId);
	
	if (nCallId >= KLtsyMinCallId && nCallId <= KLtsyMaxCallId)
		{
		TLtsyCallInformation& tCallInfo(iGlobalPhone.GetCallInfoManager().GetCallInformationByCallId(nCallId));
		if (TLtsyCallInformation::EUsed == tCallInfo.GetCallIdIsUsedInfo())
			{
			//Notify disconnecting status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing,
																				 nCallId,
																				 RMobileCall::EStatusDisconnecting);			
			//Notify idel status
			iCtsyDispatcherCallback.CallbackCallControlNotifyCallStatusChangeInd(KErrGsmCCNormalCallClearing,
																				 nCallId,
																				 RMobileCall::EStatusIdle);
			
			iGlobalPhone.GetCallInfoManager().ResetCallInformationByCallId(nCallId);
			}
		}	
	}

void CUnsolicitedCommandCallControlHandler::UnsolicitedCommandArriving(TInt aEvent, 
																	   const TDesC8& aWindPkg)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CUnsolicitedCommandCallControlHandler::UnsolicitedCommandArriving()"));
	LOGTEXT2(_L8("[Ltsy CallControl] nEvent = %d"), aEvent);
	
	switch (aEvent)
		{
		case KLtsyUnsolicitedEvent00:
			HandleSimRemoveEvent();
			break;
			
		case KLtsyUnsolicitedEvent03:
			HandleProductInitEvent();
			break;
			
		case KLtsyUnsolicitedEvent08:
			HandleNetworkLostEvent();
			break;
			
		case KLtsyUnsolicitedEvent05:
			HandleCallCreatedEvent(aWindPkg);
			break;
			
		case KLtsyUnsolicitedEvent06:
			HandleCallReleasedEvent(aWindPkg);
			break;
			
		default:
			break;
		}
	
	LOGTEXT(_L8("[Ltsy CallControl] End CUnsolicitedCommandCallControlHandler::UnsolicitedCommandArriving()"));
	}

//End of file
