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
// CATSelectNetworkMode


#include "atselectnetworkmode.h"
#include "mslogger.h"


_LIT8(KATSelectNetworkMode,"AT+COPS=%d\r");
// Class CATSelectNetworkMode
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetworkMode* CATSelectNetworkMode::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATSelectNetworkMode* self = new(ELeave) CATSelectNetworkMode(aGloblePhone,
			                                                      aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::CATSelectNetworkMode
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetworkMode::CATSelectNetworkMode(CGlobalPhonemanager& aGloblePhone, 
	                                      CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::~CATSelectNetworkMode
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetworkMode::~CATSelectNetworkMode()
	{
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::ExecuteCommand()
	{
	LOGTEXT(_L8("CATSelectNetworkMode::ExecuteCommand called"));
	iTxBuffer.Format(KATSelectNetworkMode,iNetworkSelection);
	Write();
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATSelectNetworkMode::SetNetworkSelection
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::SetNetworkSelection(const TInt aNetworkSelection)
	{
	iNetworkSelection = aNetworkSelection;
	}
// ---------------------------------------------------------------------------
// CATSelectNetworkMode::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetworkMode::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	switch (iNetworkSelection)
		{
	case 0:
		iCtsyDispatcherCallback.CallbackPhoneSetNetworkSelectionSettingComp(aStatus, 
				                RMobilePhone::ENetworkSelectionAutomatic);
		break;
	case 1:
		iCtsyDispatcherCallback.CallbackPhoneSetNetworkSelectionSettingComp(aStatus, 
				                RMobilePhone::ENetworkSelectionManual);
		break;
	default:
		iCtsyDispatcherCallback.CallbackPhoneSetNetworkSelectionSettingComp(aStatus, 
				                RMobilePhone::ENetworkSelectionUnknown);
		break;
		}
	}
//
// End of file





