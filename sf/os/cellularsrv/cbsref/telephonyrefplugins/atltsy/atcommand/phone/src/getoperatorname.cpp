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
// CGetOperatorNmae

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include "getoperatorname.h"
#include "mslogger.h"
#include "atgetnetworkinfo.h"


_LIT8(KATSetFormatForOperater, "AT+COPS=3,0\r");
// Class CGetOperatorNmae
// ---------------------------------------------------------------------------
// CGetOperatorNmae::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetOperatorNmae* CGetOperatorNmae::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CGetOperatorNmae* self = new(ELeave) CGetOperatorNmae(aGloblePhone,
			                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self ;
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::CGetOperatorNmae
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetOperatorNmae::CGetOperatorNmae(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::ConstructL()
	{
	iATCommandName = EATNone;
	iRequestIpc = 0;
	iATHelperCommand = CATHelperCommand::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATNetworkInfo = CATNetworkInfo::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::~CGetOperatorNmae
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetOperatorNmae::~CGetOperatorNmae()
	{
	delete iATHelperCommand;
	iATHelperCommand = NULL;
	delete iATNetworkInfo;
	iATNetworkInfo = NULL;
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus)
	{
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	switch(iATType)
		{
		case ELtsyAT_Phone_SetOperFormat: 
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATHelperCommand->GetErrorValue();
				}
			SetOperFormatComplete(aStatus);
			break;
		case ELtsyAT_Phone_NwkInfo:
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATNetworkInfo->GetErrorValue();
				if(KErrNone == aStatus)
					{
					iNetworkInfo = iATNetworkInfo->GetNetWorkInfo();
					}
				}
			iPhoneGlobals.iEventSignalActive = EFalse;
			iATNetworkInfo->RemoveAtCommandObserver();
			EndATCommand(iATNetworkInfo);
			CtsyDispatcherCallBack(aStatus);
			break;
		default:
			break;
		}
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::StartRequest()
	{
	iATHelperCommand->SetAtCommandObserver(this);
	iATHelperCommand->SetAtCommandString(KATSetFormatForOperater);
	iATHelperCommand->ExecuteCommand();
	iATType = ELtsyAT_Phone_SetOperFormat;
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::SetRequestTypeL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::SetRequestTypeL(TInt aIpc)
	{
	iRequestIpc = aIpc;
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::SetOperFormatComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::SetOperFormatComplete(const TInt aError)
	{
	if(KErrNone == aError)
		{
		EndATCommand(iATHelperCommand);
		StartATCommand(iATNetworkInfo);
		}
	else
		{
		EndATCommand(iATHelperCommand);
		CtsyDispatcherCallBack(aError);
		}
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::CtsyDispatcherCallBackL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::CtsyDispatcherCallBack(const TInt aError)
	{
	switch(iRequestIpc)
		{
		case MLtsyDispatchPhoneGetOperatorName::KLtsyDispatchPhoneGetOperatorNameApiId:
			iCtsyDispatcherCallback.CallbackPhoneGetOperatorNameComp(aError,  
					                RMmCustomAPI::EOperatorNameProgrammableLatin, 
					                iNetworkInfo.iLongName);
			break;
		case MLtsyDispatchPhoneGetNetworkProviderName::KLtsyDispatchPhoneGetNetworkProviderNameApiId:
			iCtsyDispatcherCallback.CallbackPhoneGetNetworkProviderNameComp(aError,  
					                iNetworkInfo.iLongName);
			break;
		case MLtsyDispatchPhoneGetServiceProviderName::KLtsyDispatchPhoneGetServiceProviderNameApiId:
			{
			RMobilePhone::TMobilePhoneServiceProviderNameV2 ServiceProvider;
			ServiceProvider.iSPName = iNetworkInfo.iLongName;
			iCtsyDispatcherCallback.CallbackPhoneGetServiceProviderNameComp(aError,ServiceProvider);
			}
			//CallbackPhoneGetServiceProviderNameComp(TInt aError, const RMobilePhone::TMobilePhoneServiceProviderNameV2& aServiceProvider);
			break;
		default:
			break;
			
		}
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::StartATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->SetAtCommandObserver(this);
	iATType = aAtCommand->AtType();
	aAtCommand->StartRequest();
	}
// ---------------------------------------------------------------------------
// CGetOperatorNmae::EndATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetOperatorNmae::EndATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	}
//
// End of file
