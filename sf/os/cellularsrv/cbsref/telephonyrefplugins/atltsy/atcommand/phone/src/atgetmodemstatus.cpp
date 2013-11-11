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

#include "atgetmodemstatus.h"
#include "mslogger.h"


_LIT8(KATCommand, "AT\r");
// Class CATGetModemStatus
// ---------------------------------------------------------------------------
// CATGetModemStatus::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModemStatus* CATGetModemStatus::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetModemStatus* self = new(ELeave) CATGetModemStatus(aGloblePhone,
			                                                aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::CATGetModemStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModemStatus::CATGetModemStatus(CGlobalPhonemanager& aGloblePhone, 
	                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModemStatus::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_ModemStatus;
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::~CATGetModemStatus
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetModemStatus::~CATGetModemStatus()
	{
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModemStatus::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModemStatus::ExecuteCommand()
	{
	iTxBuffer.Copy(KATCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModemStatus::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetModemStatus::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetModemStatus::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetModemStatus::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone !=aStatus)
		{
		iCtsyDispatcherCallback.CallbackPhoneBootNotifyModemStatusReadyComp(aStatus,ERfsStateInfoInactive);
		}
	else 
		{
		if(aEventSource == EReadCompletion)
			{
			iCtsyDispatcherCallback.CallbackPhoneBootNotifyModemStatusReadyComp(iError,ERfsStateInfoNormal);
			}
		else
			{
			return;
			}
		}
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}
//
// End of file
