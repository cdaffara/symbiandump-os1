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
// 
// 

// use include
#include "atmanager.h"
#include "activecommandstore.h"
#include "mslogger.h"

// ---------------------------------------------------------------------------
// CAtManager::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------	
CAtManager* CAtManager::NewL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::NewL()"));
	CAtManager *self = CAtManager::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CAtManager::NewLC
// other items were commented in a header
// ---------------------------------------------------------------------------	
CAtManager* CAtManager::NewLC()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::NewLC()"));
	CAtManager *self = new (ELeave) CAtManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CAtManager::~CAtManager
// other items were commented in a header
// ---------------------------------------------------------------------------	
CAtManager::~CAtManager()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::~CAtManager()"));
	
	delete iActiveCommandStore;
	iActiveCommandStore = NULL;
	}

// ---------------------------------------------------------------------------
// CAtManager::CAtManager
// other items were commented in a header
// ---------------------------------------------------------------------------	
CAtManager::CAtManager()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::CAtManager()"));
	}

// ---------------------------------------------------------------------------
// CAtManager::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtManager::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::ConstructL()"));
	iActiveCommandStore = CActiveCommandStore::NewL();
	}

// ---------------------------------------------------------------------------
// CAtManager::SetSolicitedAtCommand
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtManager::SetSolicitedAtCommand(CAtCommandBase* aSolicitedCommand)
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::SetSolicitedAtCommand()"));
	iSolicitedCommand = aSolicitedCommand;
	}

// ---------------------------------------------------------------------------
// CAtManager::GetActiveCommandStore
// other items were commented in a header
// ---------------------------------------------------------------------------	
CActiveCommandStore* CAtManager::GetActiveCommandStore() const
	{
	return iActiveCommandStore;
	}

// ---------------------------------------------------------------------------
// CAtManager::ReadOrWriteComplete
// other items were commented in a header
// ---------------------------------------------------------------------------	
void CAtManager::ReadOrWriteComplete(TAtEventSource aEvent, TInt aStatus)
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::AtCommandReadOrWriteComplete()"));
	LOGTEXT3(_L8("[Ltsy] aEvent = %d, aStatus = %d"), aEvent, aStatus);
	
    if (aEvent == EWriteCompletion)
		{
		if (iSolicitedCommand != NULL)
			{
			if(aStatus == KErrNone)
				{
				// add this active AT Command in the first place of the store
				iActiveCommandStore->AddSolicitedAtCommand(iSolicitedCommand);
				}
			else
				{
				iSolicitedCommand = NULL;
				}
			
			iSolicitedCommand->GenericEventSignal(aEvent, aStatus);
			}
		}
    else if (aEvent == EReadCompletion)
		{
		if (aStatus != KErrNone)
			{
			// check if there is active solicited AT command in the AT active command store
			if((iActiveCommandStore->GetActiveCommandArray())[0] != NULL)
	          {
	          iSolicitedCommand->GenericEventSignal(aEvent, aStatus);
	          }
			
			iSolicitedCommand = NULL;
			}
		}
    else if(aEvent == ETimeoutCompletion)
    	{
		if(iSolicitedCommand)
			{
			iSolicitedCommand->GenericEventSignal(ETimeoutCompletion, aStatus);
			iSolicitedCommand = NULL;
			}
    	}
	}

// ---------------------------------------------------------------------------
// CAtManager::NotifyOneLineFoundL
// other items were commented in a header
// ---------------------------------------------------------------------------	
TBool CAtManager::NotifyOneLineFoundL(TInt aStatus, const TDesC8& aLineBuf)
	{
	LOGTEXT(_L8("[Ltsy] Starting CAtManager::NotifyOneLineFoundL()"));
		
	RPointerArray<CAtCommandBase>& atCommands = iActiveCommandStore->GetActiveCommandArray();
	TInt count = atCommands.Count();
	LOGTEXT2(_L8("[Ltsy] count = %d"), count);
	
	TBool atCommandFound(EFalse);
	
	for (TInt n = 0; !atCommandFound && n < count; n++)
		{
		CAtCommandBase* atCommand = atCommands[n];
		if ((atCommand != NULL) && atCommand->MatchExpectString(aLineBuf))
			{
			atCommandFound = ETrue;
			atCommand->ParseResponseL(aLineBuf);
			atCommand->GenericEventSignal(EReadCompletion, aStatus);
			}
		}
	LOGTEXT(_L8("[Ltsy] End CAtManager::NotifyOneLineFoundL()"));
	return ETrue;
	}

// End of file
