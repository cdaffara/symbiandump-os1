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
// @file atcommandbase.cpp
// This contains CAtCommandBase which is the base class for AT command
// 

// user include
#include "mslogger.h"
#include "atcommandbase.h"
#include "commengine.h"
#include "atmanager.h"
#include "stringparser.h"
#include "atcommandexecuteobserver.h"
#include "activecommandstore.h"
#include "atswitchonlinemode.h"

// ---------------------------------------------------------------------------
// CAtCommandBase::~CAtCommandBase
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtCommandBase::~CAtCommandBase()
	{
	iExpectingArray.Close();
	
	delete iParser;
	iParser = NULL;
	
	if(iATSwitchOnLineMode)
		{
		delete iATSwitchOnLineMode;
		iATSwitchOnLineMode = NULL;
		}
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::AtType
// other items were commented in a header
// ---------------------------------------------------------------------------
TLtsyATCommandType CAtCommandBase::AtType()
	{
	return iAtType;
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::CAtCommandBase
// other items were commented in a header
// ---------------------------------------------------------------------------
CAtCommandBase::CAtCommandBase(CGlobalPhonemanager& aGloblePhone, 
	                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
                               :CRequestBase(aGloblePhone),
                               iCtsyDispatcherCallback(aCtsyDispatcherCallback)
					  
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtCommandBase::CAtCommandBase()"));
	
	iAtType = ELtsyAt_User_NotUse;
	
	iWriteTimeOut = KLtsyDefaultWriteTimeOut;
	iReadTimeOut = KLtsyDefaultReadTimeOut;
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtCommandBase::ConstructL()
	{
	iParser = new (ELeave) CRespondBufParser;
	iAtCommandObserver = NULL;
	//Add OK and ERROR expecting string
	iExpectingArray.AppendL(KLtsyOkString());
	iExpectingArray.AppendL(KLtsyErrorString());
	iExpectingArray.AppendL(KLtsyCmeErrorString());
	
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::AddExpectStringL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CAtCommandBase::AddExpectStringL(const TDesC8& aExpectStr)
	{
	iExpectingArray.AppendL(aExpectStr);
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::RemoveAllExpectString
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::RemoveAllExpectString()
	{
	iExpectingArray.Reset();
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::RemoveExpectString
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::RemoveExpectString(const TDesC8& aExpectStr)
	{
	TBool found(ETrue);
	TInt count = iExpectingArray.Count();
	
	for (TInt n = 0; found && n < count; n++)
		{
		if (iExpectingArray[n].Compare(aExpectStr) == 0)
			{
			iExpectingArray.Remove(n);
			iExpectingArray.Compress();
			found = EFalse;
			}
		}
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::MatchExpectString
// other items were commented in a header 
// ---------------------------------------------------------------------------		
TBool CAtCommandBase::MatchExpectString(const TDesC8& aReadLine)
	{
	TInt count = iExpectingArray.Count();
	LOGTEXT2(_L8("[Ltsy] count = %d"), count);
	for (TInt n = 0; n < count; n++)
		{
		if (aReadLine.Match(iExpectingArray[n]) == 0)
			{
			return ETrue;
			}
		}
	if(aReadLine.Length()< KLtsyGenericBufferSize)
		{
		iNoMatchedLastLine.Copy(aReadLine);
		}
	return EFalse;
	}
// ---------------------------------------------------------------------------
// CAtCommandBase::SetAtCommandObserver
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::SetAtCommandObserver(MAtCommandExecuteObserver* aAtCommandObserver)
	{
	iAtCommandObserver = aAtCommandObserver;
	}
// ---------------------------------------------------------------------------
// CAtCommandBase::RemoveAtCommandObserver
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::RemoveAtCommandObserver()
	{
	iAtCommandObserver = NULL;
	}
// ---------------------------------------------------------------------------
// CAtCommandBase::GenericEventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::GenericEventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtCommandBase::GenericEventSignal()"));
	if(iAtCommandObserver)
		{
		iAtCommandObserver->AtCommandExecuteComplete(aEventSource, aStatus);
		}
	else
		{
		EventSignal(aEventSource, aStatus);
		}
	if((aEventSource == EWriteCompletion) && (aStatus == KErrNone))
	    {
	    StartTimer(iReadTimeOut);
		}
	LOGTEXT(_L8("[Ltsy] End CAtCommandBase::GenericEventSignal()"));
	}
// ---------------------------------------------------------------------------
// CAtCommandBase::Write
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::Write()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtCommandBase::Write()"));
	if(iPhoneGlobals.iPhoneStatus.iMode==RPhone::EModeOnlineData)
		{
		if(!iATSwitchOnLineMode)
			{
			TRAPD(ret,iATSwitchOnLineMode = CATSwitchOnLineMode::NewL(iPhoneGlobals,iCtsyDispatcherCallback));
			if(ret!=KErrNone)
				{
				EventSignal(EWriteCompletion,ret);
				return;
				}
			}
		iATSwitchOnLineMode->StartSwitchOnLineMode(this,iAtCommandObserver);
		}
	else
		{
		ClearBuffer();
		iPhoneGlobals.iAtManager->SetSolicitedAtCommand(this);
		iPhoneGlobals.iCommEngine->CommWrite(iTxBuffer);	
		StartTimer(iWriteTimeOut);
		}
	}

// ---------------------------------------------------------------------------
// CAtCommandBase::SetTimeOut
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::SetTimeOut(TInt aWriteTimeOut, TInt aReadTimeOut)
	{
	iWriteTimeOut = aWriteTimeOut;
	iReadTimeOut = aReadTimeOut;
	}

//---------------------------------------------------------------------------
// CAtCommandBase::SetTimeOut
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::StartTimer(const TInt aTimeOut)
	{
	iPhoneGlobals.iCommEngine->StartTimer(aTimeOut);
	}

//---------------------------------------------------------------------------
// CAtCommandBase::StopTimer
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::StopTimer()
	{
	iPhoneGlobals.iCommEngine->StopTimer();
	}

//---------------------------------------------------------------------------
// CAtCommandBase::Buffer
// other items were commented in a header
// ---------------------------------------------------------------------------		
TPtrC8 CAtCommandBase::Buffer() const
	{
	return iPhoneGlobals.iCommEngine->GetStringParse()->Buffer();
	}

//---------------------------------------------------------------------------
// CAtCommandBase::CurrentLine
// other items were commented in a header
// ---------------------------------------------------------------------------		
TPtrC8 CAtCommandBase::CurrentLine() const
	{
	return iPhoneGlobals.iCommEngine->GetStringParse()->CurrentLine();
	}
//---------------------------------------------------------------------------
// CAtCommandBase::PrecedingLine
// other items were commented in a header
// ---------------------------------------------------------------------------		
TPtrC8 CAtCommandBase::PrecedingLine() const
	{
	return iNoMatchedLastLine;
	}
//---------------------------------------------------------------------------
// CAtCommandBase::ClearBuffer
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::ClearBuffer()
	{
	iPhoneGlobals.iCommEngine->GetStringParse()->ClearBuffer();
	}

//---------------------------------------------------------------------------
// CAtCommandBase::ClearCurrentLine
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::ClearCurrentLine()
	{
	iPhoneGlobals.iCommEngine->GetStringParse()->ClearCurrentLine();	
	}

//---------------------------------------------------------------------------
// CAtCommandBase::Complete
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::Complete()
    {
    (iPhoneGlobals.iAtManager->GetActiveCommandStore())->RemoveActiveAtCommand(this);	
	StopTimer();
	ClearBuffer();
	}

//---------------------------------------------------------------------------
// CAtCommandBase::CancelCommand
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::CancelCommand()
	{
	
	}

//---------------------------------------------------------------------------
// CAtCommandBase::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------		
void CAtCommandBase::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtCommandBase::ParseResponseL()"));
	}

void CAtCommandBase::EventSignal(TAtEventSource /*aEventSource*/, TInt /*aStatus*/)
	{
	
	}
//---------------------------------------------------------------------------
// CAtCommandBase::AddUnsolicitedAtCommand
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::AddUnsolicitedAtCommand()
	{
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->AddUnsolicitedAtCommand(this);
	}
//---------------------------------------------------------------------------
// CAtCommandBase::RemoveAtCommand
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtCommandBase::RemoveAtCommand()
	{
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->RemoveActiveAtCommand(this);
	}
// End of file
