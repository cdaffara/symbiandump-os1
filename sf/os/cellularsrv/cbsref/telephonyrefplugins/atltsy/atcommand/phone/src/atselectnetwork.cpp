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
// CATSelectNetwork

#include "atselectnetwork.h"
#include "mslogger.h"

_LIT8(KSelectNetworkCommand,"AT+COPS=1,2,%d\r");
// Class CATSelectNetwork
// ---------------------------------------------------------------------------
// CATSelectNetwork::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetwork* CATSelectNetwork::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATSelectNetwork* self = new(ELeave) CATSelectNetwork(aGloblePhone,
			                                              aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::CATSelectNetwork
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetwork::CATSelectNetwork(CGlobalPhonemanager& aGloblePhone, 
	                               CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iCommandRunning = EFalse;
	iCommandCancel = EFalse;
	iCommandSetBack = EFalse;
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::~CATSelectNetwork
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSelectNetwork::~CATSelectNetwork()
	{
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::StartRequest()
	{
	iCommandRunning = ETrue;
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::ExecuteCommand()
	{
	LOGTEXT(_L8("CATSelectNetwork::ExecuteCommand called"));
	TBuf<5> tmpbuf;
	tmpbuf.Append(iNetworkManualSelection.iCountry);
	tmpbuf.Append(iNetworkManualSelection.iNetwork);
    TLex lex(tmpbuf);
    TInt tmpNum;
    lex.Val( tmpNum );
	LOGTEXT2(_L8("tmpNum=%d"),tmpNum);
	iTxBuffer.Format(KSelectNetworkCommand,tmpNum);
	Write();
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::CancelCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::CancelCommand()
	{	
	iCommandCancel = ETrue;
	}

// ---------------------------------------------------------------------------
// CATSelectNetwork::ExecuteSetBackCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::ExecuteSetBackCommand()
	{
	LOGTEXT(_L8("CATSelectNetwork::ExecuteSetBackCommand called"));
	TBuf<5> tmpbuf;
	tmpbuf.Append(iPhoneGlobals.iPhoneStatus.iCurrentNetwork.iCountryCode);
	tmpbuf.Append(iPhoneGlobals.iPhoneStatus.iCurrentNetwork.iNetworkId);
    TLex lex(tmpbuf);
    TInt tmpNum;
    lex.Val( tmpNum );
	LOGTEXT2(_L8("tmpNum=%d"),tmpNum);
	iTxBuffer.Format(KSelectNetworkCommand,tmpNum);
	Write();
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATSelectNetwork::SetNetworkSelection
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::SetNetworkSelection(const RMobilePhone::TMobilePhoneNetworkManualSelection& aNetworkManualSelection)
	{
	iNetworkManualSelection = aNetworkManualSelection;
	}
// ---------------------------------------------------------------------------
// CATSelectNetwork::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSelectNetwork::EventSignal(TAtEventSource aEventSource, TInt aStatus)
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
	if(iCommandCancel) //User runing cancel cmd
		{
		//We should set back network when selected network success
		if((KErrNone ==aStatus)&&(!iCommandSetBack))
			{
			iCommandSetBack = ETrue;
			//Set parameter
			ExecuteSetBackCommand();
			return;
			}
		if((iCommandSetBack)&&(KErrNone !=aStatus))
			{
			//Cancel fail because set back fail 
			//Current Network Changed
			iCtsyDispatcherCallback.CallbackPhoneSelectNetworkCancelComp(aStatus);
			}
		else
			{
			//First selectrd fail or set back success
			//Cancel OK
			iCtsyDispatcherCallback.CallbackPhoneSelectNetworkCancelComp(KErrNone);
			}
		}
	else
		{
		iCtsyDispatcherCallback.CallbackPhoneSelectNetworkComp(aStatus);
		}
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCommandRunning = EFalse;
	iCommandCancel = EFalse;
	iCommandSetBack = EFalse;
	}
//
// End of file


