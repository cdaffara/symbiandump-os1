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
// This contains CATSwitchOnLineMode which switch to online mode
// 

#include "atswitchonlinemode.h"
#include "mslogger.h"
#include "commengine.h"
#include "tsyconfg.h"

// Class CATSwitchOnLineMode
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSwitchOnLineMode* CATSwitchOnLineMode::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATSwitchOnLineMode* self = new(ELeave) CATSwitchOnLineMode(aGloblePhone,
			                                                    aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::CATSwitchOnLineMode
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSwitchOnLineMode::CATSwitchOnLineMode(CGlobalPhonemanager& aGloblePhone, 
	                                     CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::ConstructL()
	{
	CAtCommandBase::ConstructL();
	RemoveAllExpectString();
	AddExpectStringL(KLtsyNoCarrierString());
	iWhichTimer = ETimerNone;
	iCallbackTimer = CCallbackTimer::NewL(*this);
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::~CATSwitchOnLineMode
// other items were commented in a header
// ---------------------------------------------------------------------------
CATSwitchOnLineMode::~CATSwitchOnLineMode()
	{
	if(iCallbackTimer)
		{
		iCallbackTimer->Cancel();
		delete iCallbackTimer;
		iCallbackTimer = NULL;
		}
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::ExecuteCommand()
	{
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if(CurrentLine().Match(KLtsyNoCarrierString) == 0)
		{
		iError = KErrNone;
		}
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::StartSwitchOnLineModeL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::StartSwitchOnLineMode(CAtCommandBase* aAtCommandBase,
		                  MAtCommandExecuteObserver* aAtCommandObserver)
	{
	iAtCommandBase = aAtCommandBase;
	iAtCommandObserver = aAtCommandObserver;
	RemoveAtCommandObserver();
	iPhoneGlobals.iCommEngine->CommCancel();		// This required to avoid a panic in C32 when we call iIo->ConfigurePort
	TCommConfig configPckg;
	TInt ret = KErrNone;
	ret = iPhoneGlobals.iConfiguration->PortConfig(configPckg,EConfigTypeHangUp);	
	if (ret==KErrNone)
		ret = iPhoneGlobals.iCommEngine->ConfigurePort(configPckg);		
	if (ret!=KErrNone)
		{
		iAtCommandBase->EventSignal(EWriteCompletion,ret);
		}
	iPhoneGlobals.iCommEngine->DropDtr();
	iWhichTimer = ETimerDrop;
	StartTimer(500000L);
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::EventSignal(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		aStatus = iError;
		}
	CAtCommandBase::Complete();
	if(KErrNone ==aStatus)
		{
		//iPhoneGlobals.iPhoneStatus.iMode = RPhone::EModeOnlineCommand;
		Disable();
		StartTimer(1000000L);
		iWhichTimer = ETimerWait;
		}
	else
		{
		Disable();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iAtCommandBase->EventSignal(EWriteCompletion,KErrNotReady);
		}
	
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::Enable
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::Enable()
	{
	AddUnsolicitedAtCommand();
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::Disable
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::Disable()
	{
	RemoveAtCommand();
	}
// ---------------------------------------------------------------------------
// CATSwitchOnLineMode::StartTimer
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATSwitchOnLineMode::StartTimer(const TInt aTimeout)
	{
	if (iCallbackTimer->IsActive())
		iCallbackTimer->Cancel();
	iCallbackTimer->After(aTimeout);
	}
/**
 * 
 * 
 */
void CATSwitchOnLineMode::TimerRun(TInt aError)
    {
	if(aError == KErrNone)
	    {
		switch(iWhichTimer)
			{
			case ETimerDrop:
			    LOGTEXT(_L8("[Ltsy] CATSwitchOnLineMode::ETimerDrop time out."));
				iPhoneGlobals.iCommEngine->RaiseDTR();
				iPhoneGlobals.iPhoneStatus.iMode = RPhone::EModeOnlineCommand;
				iPhoneGlobals.iEventSignalActive = ETrue;
				iWhichTimer = ETimerRaise;
				StartTimer(1000000L);
				break;
			case ETimerRaise:
                LOGTEXT(_L8("[Ltsy] CATSwitchOnLineMode::ETimerRaise time out."));
				iPhoneGlobals.iCommEngine->ResetBuffers();
				iWhichTimer = ETimerReset;
				StartTimer(1000000L);
				break;
			case ETimerReset:
                LOGTEXT(_L8("[Ltsy] CATSwitchOnLineMode::ETimerReset time out."));
				ClearBuffer();
				iPhoneGlobals.iCommEngine->Read();
				iWhichTimer = ETimerRead;
				Enable();
				StartTimer(1000000L);
				break;
			case ETimerRead:
                LOGTEXT(_L8("[Ltsy] CATSwitchOnLineMode::ETimerRead time out."));
				StartTimer(1000000L);
				iWhichTimer = ETimerWait;
				break;
			case ETimerWait:
                LOGTEXT(_L8("[Ltsy] CATSwitchOnLineMode::ETimerWait time out."));
			    if(iAtCommandObserver)
			    	{
			    	SetAtCommandObserver(iAtCommandObserver);
			    	}
			    Disable();
			    iAtCommandBase->StartRequest(); 
			    iWhichTimer = ETimerNone;
				break;
			default:
				Disable();
				iPhoneGlobals.iEventSignalActive = EFalse;
				iAtCommandBase->EventSignal(EWriteCompletion,KErrNotReady);
				break;
			}
		}
	else
		{
		Disable();
		iPhoneGlobals.iEventSignalActive = EFalse;
		iAtCommandBase->EventSignal(EWriteCompletion,KErrNotReady);
		}
	}
//
// End of file
