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
// CGetFdnStatus

#include "getfdnstatus.h"
#include "mslogger.h"
#include "atgetfdnsupport.h"
#include "atgetfdnstatus.h"

// Class CGetFdnStatus
// ---------------------------------------------------------------------------
// CGetFdnStatus::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetFdnStatus* CGetFdnStatus::NewL(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CGetFdnStatus* self = new(ELeave) CGetFdnStatus(aGloblePhone,
			                                        aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self ;
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::CGetFdnStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetFdnStatus::CGetFdnStatus(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CATCommandControlBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::ConstructL()
	{
	iATCommandName = EATNone;
	iATGetFdnStatus = CATGetFdnStatus::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	iATGetFdnSupport = CATGetFdnSupport::NewL(iPhoneGlobals,iCtsyDispatcherCallback);
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::~CGetFdnStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CGetFdnStatus::~CGetFdnStatus()
	{
	delete iATGetFdnStatus;
	iATGetFdnStatus = NULL;
	delete iATGetFdnSupport;
	iATGetFdnSupport = NULL;
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::AtCommandExecuteComplete(TAtEventSource aEventSource, TInt aStatus)
	{
	if((aEventSource == EWriteCompletion)&&(KErrNone == aStatus))
		{
		return;
		}
	switch(iATType)
		{
		case ELtsyAT_Phone_CheckFdnSupport:
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetFdnSupport->GetErrorValue();
				}
			iATGetFdnSupport->RemoveAtCommandObserver();
			FdnSupportComplete(aStatus);
			break;
		case ELtsyAT_Phone_GetFdnStatus: 
			if((KErrNone == aStatus)&&(aEventSource == EReadCompletion))
				{
				aStatus = iATGetFdnStatus->GetErrorValue();
				}
			FdnStatusComplete(aStatus);
			EndATCommand(iATGetFdnStatus);
			delete iATGetFdnStatus;
			iATGetFdnStatus = NULL;
			iPhoneGlobals.iEventSignalActive = EFalse;
			break;
		default:
			break;
		}
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::StartRequest()
	{
	StartATCommand(iATGetFdnSupport);
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::HandleFdnSupport
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::FdnSupportComplete(const TInt aError)
	{
	if((KErrNone == aError)&&(iATGetFdnSupport->GetFdnSupport()))
		{	
		EndATCommand(iATGetFdnSupport);
		StartATCommand(iATGetFdnStatus);
		}
	else
		{
		EndATCommand(iATGetFdnSupport);
		iPhoneGlobals.iEventSignalActive = EFalse;
		iCtsyDispatcherCallback.CallbackPhoneGetFdnStatusComp(aError,RMobilePhone::EFdnUnknown);
		}
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::FdnStatusComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::FdnStatusComplete(const TInt aError)
	{
	if((KErrNone == aError)&&(iATGetFdnStatus->GetFdnStatus()))
		{
		iCtsyDispatcherCallback.CallbackPhoneGetFdnStatusComp(aError,RMobilePhone::EFdnActive);
		}
	else
		{
		iCtsyDispatcherCallback.CallbackPhoneGetFdnStatusComp(aError,RMobilePhone::EFdnNotActive);
		}
	}

// ---------------------------------------------------------------------------
// CGetFdnStatus::StartATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::StartATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->SetAtCommandObserver(this);
	iATType = aAtCommand->AtType();
	aAtCommand->StartRequest();
	}
// ---------------------------------------------------------------------------
// CGetFdnStatus::EndATCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CGetFdnStatus::EndATCommand(CAtCommandBase* aAtCommand)
	{
	aAtCommand->RemoveAtCommandObserver();
	aAtCommand->Complete();
	}
//
// End of file
