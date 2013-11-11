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
// CATGetModemStatus
// Description:
// CInitializePhone

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "initializephone.h"
#include "mslogger.h"
#include "atgetmodemstatus.h"
#include "atgetnetworkregistrationstatus.h"
#include "atgetsimstatusready.h"
#include "getcurrentnetworkinfo.h"
#include "atgetnetworkinfo.h"
#include "atgetsignalstrength.h"
#include "atgetbatteryinfo.h"
#include "athelpercommand.h"
#include "notifybatteryandsignal.h"
#include "commengine.h"
#include "atnetworkregstatuschange.h"

_LIT8(KATSMSSetting1,              "AT+CMGF=0\r");
_LIT8(KATSMSSetting2,              "AT+CNMI=2,2\r");
_LIT8(KATNwkRegistration,          "AT+CREG=1\r");
_LIT8(KATGprsNwkRegistration,      "AT+CGREG=1\r");
_LIT8(KATUnsolicitedWind,          "AT+WIND=16383\r");
_LIT8(KATSetTDRSingal,             "AT&D1\r");
// Class CInitializePhone
// ---------------------------------------------------------------------------
// CInitializePhone::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitializePhone* CInitializePhone::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback,
	                                     CATNetworkRegStatusChange& aNtwkRegStatusChange,
	                                     CNotifyBatteryAndSignal& aNotifyBatteryAndSignal)
	{
	CInitializePhone* self = new(ELeave) CInitializePhone(aGloblePhone,
			                                              aCtsyDispatcherCallback,
			                                              aNtwkRegStatusChange,
			                                              aNotifyBatteryAndSignal);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self ;
	}

// ---------------------------------------------------------------------------
// CInitializePhone::CInitializePhone
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitializePhone::CInitializePhone(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback,
	                               CATNetworkRegStatusChange& aNtwkRegStatusChange,
	                               CNotifyBatteryAndSignal& aNotifyBatteryAndSignal)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback),
		iNtwkRegStatusChange(aNtwkRegStatusChange),
		iNotifyBatteryAndSignal(aNotifyBatteryAndSignal)
	{
	}

// ---------------------------------------------------------------------------
// CInitializePhone::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::ConstructL()
	{
	iATGetCurNetworkInfo = CGetCurrentNetworkInfo::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetCurNetworkInfo->SetIpcRequestCompleteObserver(this);
	
	iATHelperCommand = CATHelperCommand::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iATGetModemStatus = CATGetModemStatus::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iATSIMCard = CATSIMCard::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iNetworkRegStatus = CATNetworkRegistration::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iNetworkRegStatus->SetIpcRequestCompleteObserver(this);
	iNetworkRegStatus->SetIpcId(MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId);
	
	iATGetSignal = CATGetSignal::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iATGetBattery = CATGetBattery::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	
	iPendingRequestList.Reset();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::~CInitializePhone
// other items were commented in a header
// ---------------------------------------------------------------------------
CInitializePhone::~CInitializePhone()
	{
	delete iATGetModemStatus;
	iATGetModemStatus = NULL;

	delete iNetworkRegStatus;
	iNetworkRegStatus = NULL;

	delete iATSIMCard;
	iATSIMCard = NULL;

	delete iATGetCurNetworkInfo;
    
	delete iATGetSignal;
	iATGetSignal = NULL;

	delete iATGetBattery;
	iATGetBattery = NULL;

	delete iATHelperCommand;
	iATHelperCommand = NULL;
	
	iPendingRequestList.Close();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus)
	{
	// we are not going to handle the event of EWriteCompletion if no error was returned
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	
	TInt err = aStatus;
	switch(iATType)
		{
		case ELtsyAT_Phone_ModemStatus:
		    if(aStatus == KErrNone)
		        {
		        err = iATGetModemStatus->GetErrorValue();
		        }
		    
            ModemStatusComplete(err);
            CompleteATCommand(iATGetModemStatus);
			break;
			
		case ELtsyAT_Phone_NwkRegStatus: 
			if(KErrNone == aStatus)
				{
				err = iNetworkRegStatus->GetErrorValue();
				}
			iCtsyDispatcherCallback.CallbackPhoneGetNetworkRegistrationStatusComp(err,
								    iPhoneGlobals.iPhoneStatus.iRegistrationStatus);
			CompleteATCommand(iNetworkRegStatus);
			break;
			
		case ELtsyAT_Phone_SimCardStatus: 
			if(KErrNone == aStatus)
				{
				err = iATSIMCard->GetErrorValue();
				}
			iCtsyDispatcherCallback.CallbackPhoneBootNotifySimStatusReadyComp(err);
			CompleteATCommand(iATSIMCard);
			break;
			
		case ELtsyAT_Phone_GetSignal: 
			if(KErrNone == aStatus)
				{
				err = iATGetSignal->GetErrorValue();
				}
			GetSignalComplete(err);
			CompleteATCommand(iATGetSignal);
			break;
			
		case ELtsyAT_Phone_GetBattery:
			if(KErrNone == aStatus)
				{
				err = iATGetBattery->GetErrorValue();
				}
			GetBatteryComplete(err);
			CompleteATCommand(iATGetBattery);
			break;
			
		case ELtsyAT_Phone_SMSSetting1:
			ExecuteNextATHelpCommand(KATSMSSetting2,ELtsyAT_Phone_SMSSetting2);
			break;
			
		case ELtsyAT_Phone_SMSSetting2:
			ExecuteNextATHelpCommand(KATNwkRegistration,ELtsyAT_Phone_RegistNwk);
			break;
		case ELtsyAT_Phone_RegistNwk:
			ExecuteNextATHelpCommand(KATGprsNwkRegistration,ELtsyAT_Phone_GPRSRegNwk);
			break;
			
		case ELtsyAT_Phone_GPRSRegNwk:
			ExecuteNextATHelpCommand(KATSetTDRSingal,ELtsyAT_Phone_SetTDRSingal);
			break;
			
		case ELtsyAT_Phone_SetTDRSingal:
			ExecuteNextATHelpCommand(KATUnsolicitedWind,ELtsyAT_Phone_SetWind);
			break;
			
		case ELtsyAT_Phone_SetWind:
			iPhoneGlobals.iEventSignalActive = EFalse;
			if(iPhoneGlobals.iPhoneStatus.iInitStatus == EPhoneInitialising)
				{
				iPhoneGlobals.iPhoneStatus.iInitStatus = EPhoneInitialised;
				}
			CompleteATCommand(iATHelperCommand);
			break;
			
		default:
			break;
		}
	}

// ---------------------------------------------------------------------------
// CInitializePhone::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::StartRequest()
	{
	StartATCommand(iATGetModemStatus);
	}

// ---------------------------------------------------------------------------
// CInitializePhone::ExecuteNextRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::ExecutePendingRequest()
	{
	if(iPendingRequestList.Count() > 0 && !iIsPendingRequestStillRunning)
		{
		// Mark if the next pending request should be run immediately. 
		// There is a case: If we have alreay known the passed-back results for a certain API, so when such API passed by, 
		// what is going to to is to complete this IPC request immediately synchronously.
		TBool nextPendingReqShouldBeExecutedImmediatley = EFalse;
		
		// get the IPC number
		TInt ipc = iPendingRequestList[0];
		
		// remove this request from the pending request list
		iPendingRequestList.Remove(0);
		
		switch(ipc)
			{
			case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId:				
				StartATCommand(iATSIMCard);
				break;
				
			case MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId:
			    iNtwkRegStatusChange.Disable();
				StartATCommand(iNetworkRegStatus);
				break;
				
			case MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId: 
				iATGetCurNetworkInfo->StartRequest();
				break;
				
			case MLtsyDispatchPhoneGetNetworkMode::KLtsyDispatchPhoneGetNetworkModeApiId: 
				iCtsyDispatcherCallback.CallbackPhoneGetNetworkModeComp(0,RMobilePhone::ENetworkModeGsm);
				nextPendingReqShouldBeExecutedImmediatley = ETrue;
				break;
				
			case MLtsyDispatchPhoneGetNitzInfo::KLtsyDispatchPhoneGetNitzInfoApiId:
				iCtsyDispatcherCallback.CallbackPhoneGetNitzInfoComp(0,iPhoneGlobals.iPhoneStatus.iMobilePhoneNITZ);
				nextPendingReqShouldBeExecutedImmediatley = ETrue;
				break;
				
			case MLtsyDispatchPhoneGetSignalStrength::KLtsyDispatchPhoneGetSignalStrengthApiId: 
				StartATCommand(iATGetSignal);
				break;
				
			case MLtsyDispatchPhoneGetBatteryInfo::KLtsyDispatchPhoneGetBatteryInfoApiId: 
				StartATCommand(iATGetBattery);
				break;
				
			case MLtsyDispatchPhoneGetHomeNetwork::KLtsyDispatchPhoneGetHomeNetworkApiId: 
				iCtsyDispatcherCallback.CallbackPhoneGetHomeNetworkComp(0,iNetworkInfo);
				nextPendingReqShouldBeExecutedImmediatley = ETrue;
				break;
				
			case MLtsyDispatchPhoneSimRefreshRegister::KLtsyDispatchPhoneSimRefreshRegisterApiId: 
				iCtsyDispatcherCallback.CallbackPhoneSimRefreshRegisterComp(0);
				iATHelperCommand->SetAtCommandObserver(this);
				ExecuteNextATHelpCommand(KATSMSSetting1,ELtsyAT_Phone_SMSSetting1);
				break;
				
			default:
				break;
			}
		
		// if the request was complete, we need to call ExecutePendingRequest() to execute next pending request again 
        if(nextPendingReqShouldBeExecutedImmediatley)
            {
            ExecutePendingRequest();
            }
		}
	}

// ---------------------------------------------------------------------------
// CInitializePhone::NotifyIpcRequestComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::NotifyIpcRequestComplete(TInt aIpcId)
    {
    if(aIpcId == MLtsyDispatchPhoneGetCurrentNetworkInfo::KLtsyDispatchPhoneGetCurrentNetworkInfoApiId)
        {
        NetworkInfoComplete();
        }
    else if(aIpcId == MLtsyDispatchPhoneGetNetworkRegistrationStatus::KLtsyDispatchPhoneGetNetworkRegistrationStatusApiId)
        {
        iNtwkRegStatusChange.Enable();
        }
    }

// ---------------------------------------------------------------------------
// CInitializePhone::ExecuteNextATHelpCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::ExecuteNextATHelpCommand(const TDesC8& aATString,TLtsyATCommandType aATType)
	{
	iATHelperCommand->Complete();
	iATHelperCommand->SetAtCommandString(aATString);
	iATType = aATType;
	iATHelperCommand->StartRequest();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::StartATCommand(CAtCommandBase* aAtCommand)
	{
	iIsPendingRequestStillRunning = ETrue;
	aAtCommand->SetAtCommandObserver(this);
	iATType = aAtCommand->AtType();
	aAtCommand->StartRequest();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::CompleteATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::CompleteATCommand(CAtCommandBase* aAtCommand)
	{
	iIsPendingRequestStillRunning = EFalse;
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	ExecutePendingRequest();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::ModemStatusComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::ModemStatusComplete(TInt aError)
	{
	if(KErrNone == aError)
		{
		iCtsyDispatcherCallback.CallbackPhoneBootNotifyModemStatusReadyComp(aError,
				                ERfsStateInfoNormal);
		}
	else
		{
		User::After(5000000);
		iCtsyDispatcherCallback.CallbackPhoneBootNotifyModemStatusReadyComp(aError,
							    ERfsStateInfoNormal);
		}
	}

// ---------------------------------------------------------------------------
// CInitializePhone::NetworkInfoComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::NetworkInfoComplete()
	{
	ExecutePendingRequest();
	}

// ---------------------------------------------------------------------------
// CInitializePhone::GetSignalComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::GetSignalComplete(TInt aError)
	{
	if(KErrNone == aError)
		{
		iSignalInfo = iATGetSignal->getSignalInfo();
		iPhoneGlobals.iPhoneStatus.iSignalInfo = iSignalInfo;
		}
	iCtsyDispatcherCallback.CallbackPhoneGetSignalStrengthComp(aError,
							iSignalInfo.iSignalStrength,
							iSignalInfo.iBar);
	}
// ---------------------------------------------------------------------------
// CInitializePhone::GetBatteryComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::GetBatteryComplete(TInt aError)
	{
	if(KErrNone == aError)
		{
		iBatteryInfo = iATGetBattery->GetBatteryinfo();
		iPhoneGlobals.iPhoneStatus.iBatteryInfo = iBatteryInfo;
		iPhoneGlobals.iPhoneStatus.iRfStateInfo = iATGetBattery->GetRfStateInfo();
		}
	iCtsyDispatcherCallback.CallbackPhoneGetBatteryInfoComp(aError,iBatteryInfo);
	}

// ---------------------------------------------------------------------------
// CInitializePhone::GetBatteryComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CInitializePhone::AppendIpcRequestToPendingListL(TInt aIpc)
    {
    iPendingRequestList.AppendL(aIpc);
    }
//
// End of file
