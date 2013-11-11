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
// This contains CUnsolicitedEventMonitor which mornitor the arriving unsocilited event and then notify this to all observers.
// 

//user include
#include "unsolicitedeventmonitor.h"
#include "atunsolicited.h"
#include "mslogger.h"
#include "atmanager.h"
#include "activecommandstore.h"

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CUnsolicitedEventMonitor* CUnsolicitedEventMonitor::NewL(CGlobalPhonemanager& aPhoneglobals,
										    CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::NewL()"));
	CUnsolicitedEventMonitor* self = CUnsolicitedEventMonitor::NewLC(aPhoneglobals, aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------
CUnsolicitedEventMonitor* CUnsolicitedEventMonitor::NewLC(CGlobalPhonemanager& aPhoneglobals,
											CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::NewLC()"));
	CUnsolicitedEventMonitor* self = new (ELeave) CUnsolicitedEventMonitor(aPhoneglobals, 
																 aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::~CUnsolicitedEventMonitor
// other items were commented in a header
// ---------------------------------------------------------------------------
CUnsolicitedEventMonitor::~CUnsolicitedEventMonitor()
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::~CUnsolicitedEventMonitor()"));
	iArray.Close();
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::CUnsolicitedEventMonitor
// other items were commented in a header
// ---------------------------------------------------------------------------
CUnsolicitedEventMonitor::CUnsolicitedEventMonitor(CGlobalPhonemanager& aPhoneglobals,
		                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
						             :CATCommandControlBase(aPhoneglobals, aCtsyDispatcherCallback)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::CUnsolicitedEventMonitor()"));
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::ConstructL()"));
	
	CATUnsolicited* command = CATUnsolicited::NewL(iPhoneGlobals, iCtsyDispatcherCallback);
	command->SetAtCommandObserver(this);
	AddAtCommand(command);
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::AddUnsolicitedObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::AddUnsolicitedObserver(const MLtsyUnsolicitedCommandObserver* aObserver)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::AddUnsolicitedObserver()"));
	
	if (aObserver != NULL)
		{
		TInt nRes = iArray.Append(aObserver);
		if (nRes == KErrNone)
			{
			LOGTEXT(_L8("[Ltsy] Append unsolicited Observer Success"));
			}
		else
			{
			LOGTEXT2(_L8("[Ltsy] Append unsolicited Observer fail Because result code = %d"), nRes);
			}
		}
	else
		{
		LOGTEXT(_L8("[Ltsy] Unsolicited observer is NULL"));
		}
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::RemoveUnsolicitedObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::RemoveUnsolicitedObserver(const MLtsyUnsolicitedCommandObserver* aObserver)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::RemoveUnsolicitedObserver()"));
	if (aObserver != NULL)
		{
		TInt nFound = iArray.Find(aObserver);
		if (KErrNotFound != nFound)
			{
			LOGTEXT(_L8("[Ltsy] Array have removed observer"));
			iArray.Remove(nFound);
			}
		else
			{
			LOGTEXT(_L8("[Ltsy] Array not found observer"));
			}
		}
	else
		{
		LOGTEXT(_L8("[Ltsy] Wind Observer is NULL"));
		}
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::NotifyEveryObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::NotifyEveryObserver()
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::NotifyEveryObserver()"));
	
	CAtCommandBase *cAtBase(NULL);
	TRAPD(err, cAtBase = &GetAtCommandByAtTypeL(ELtsyAT_General_WIND));
	if (err == KErrNoMemory)
		{
		return;
		}
	
	CATUnsolicited*  unsolicited = dynamic_cast<CATUnsolicited*>(cAtBase);
	if (unsolicited == NULL)
		{
		LOGTEXT(_L8("[Ltsy] Dynamic cast class CAtCommandBase to class CATUnsolicited failing"));
		return;
		}
	
	if (unsolicited->GetParserResult() != KErrNone)
		{
		LOGTEXT(_L8("[Ltsy] parse unsolicited command error"));
		return;
		}
	
	TUnsolicitedParams::TUnsolicitedParamsPckgBuf* pckg = (TUnsolicitedParams::TUnsolicitedParamsPckgBuf*)(&(unsolicited->GetUnsolicitedParams()));
	TInt nEvent = (*pckg)().iEvent;
	
	TInt count = iArray.Count();
	for (TInt n = 0; n < count;  n++)
		{
		MLtsyUnsolicitedCommandObserver* observer = iArray[n];
		observer->UnsolicitedCommandArriving(nEvent, (*pckg));
		}
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::StartUnsolicitedObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::StartUnsolicitedObserver()
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::NotifyEveryObserver()"));
	
	CAtCommandBase *cAtBase(NULL);
	TRAPD(err, cAtBase = &GetAtCommandByAtTypeL(ELtsyAT_General_WIND));
	if (err == KErrNoMemory)
		{
		return;
		}
	
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->AddUnsolicitedAtCommand(cAtBase);
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::StopUnsolicitedObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::StopUnsolicitedObserver()
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::StopUnsolicitedObserver()"));
	
	CAtCommandBase *cAtBase(NULL);
	TRAPD(err, cAtBase = &GetAtCommandByAtTypeL(ELtsyAT_General_WIND));
	if (err == KErrNoMemory)
		{
		return;
		}
	
	iPhoneGlobals.iAtManager->GetActiveCommandStore()->RemoveActiveAtCommand(cAtBase);
	}

// ---------------------------------------------------------------------------
// CUnsolicitedEventMonitor::AtCommandExecuteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------
void CUnsolicitedEventMonitor::AtCommandExecuteComplete(TAtEventSource /*aEventSource*/, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CUnsolicitedEventMonitor::AtCommandExecuteComplete()"));
	if (aStatus == KErrNone)
		{
		NotifyEveryObserver();
		}
	
	CAtCommandBase *cAtBase(NULL);
	TRAPD(err, cAtBase = &GetAtCommandByAtTypeL(ELtsyAT_General_WIND));
	if (err == KErrNoMemory)
		{
		return;
		}
	
	cAtBase->ClearCurrentLine();
	
	LOGTEXT(_L8("[Ltsy] End CUnsolicitedEventMonitor::AtCommandExecuteComplete()"));
	}

//End of file
