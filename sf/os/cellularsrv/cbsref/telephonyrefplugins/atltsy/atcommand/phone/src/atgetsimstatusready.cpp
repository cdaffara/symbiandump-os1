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
// CATSIMCard

#include "atgetsimstatusready.h"
#include "mslogger.h"


_LIT8(KGetSIMCardCommand, "AT+CCID=?;\r");
// Class CATSIMCard
// ---------------------------------------------------------------------------
// CATSIMCard::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSIMCard* CATSIMCard::NewL(CGlobalPhonemanager& aGloblePhone, 
	                         CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATSIMCard* self = new(ELeave) CATSIMCard(aGloblePhone,
			                                  aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATSIMCard::CATSIMCard
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSIMCard::CATSIMCard(CGlobalPhonemanager& aGloblePhone, 
	                   CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATSIMCard::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSIMCard::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_SimCardStatus;
	}
// ---------------------------------------------------------------------------
// CATSIMCard::~CATSIMCard(
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSIMCard::~CATSIMCard()
	{
	}
// ---------------------------------------------------------------------------
// CATSIMCard::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSIMCard::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATSIMCard::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSIMCard::ExecuteCommand( )
	{
	iTxBuffer.Copy(KGetSIMCardCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATSIMCard::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSIMCard::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATSIMCard::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATSIMCard::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATSIMCard::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSIMCard::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else
			return;
		}
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhoneBootNotifySimStatusReadyComp(aStatus);
	}
//
// End of file
