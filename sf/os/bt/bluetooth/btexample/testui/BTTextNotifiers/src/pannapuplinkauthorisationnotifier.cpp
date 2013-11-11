// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pannapuplinkauthorisationnotifier.h"

#include <networking/panuiinterfaces.h>
#include <e32cons.h>
#include <btsdp.h>		// For PAN UUIDs

_LIT(KNapUplinkAuthorisationNotifierConsoleName, "PAN NAP Uplink Authorisation Notifier");
_LIT(KYes,"Yes");
_LIT(KNo,"No");

/*static*/ CPanNapUplinkAuthorisationNotifier* CPanNapUplinkAuthorisationNotifier::NewLC()
	{
	CPanNapUplinkAuthorisationNotifier* self = new (ELeave)CPanNapUplinkAuthorisationNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPanNapUplinkAuthorisationNotifier::~CPanNapUplinkAuthorisationNotifier()
	{
	delete iSelectorEngine;
	}
	
CPanNapUplinkAuthorisationNotifier::CPanNapUplinkAuthorisationNotifier()
	{
	iNotifierInfo.iUid = KBTPanNapUplinkAuthorisationNotifierUid;
	iNotifierInfo.iChannel = TUid::Uid(KScreenOutputChannelNumber);
	iNotifierInfo.iPriority = ENotifierPriorityVLow;
	}

void CPanNapUplinkAuthorisationNotifier::ConstructL()
	{
	iSelectorEngine = CPanNapUplinkAuthorisationNotifierEngine::NewL(*this);
	}
	

	// From MNotifierBase2

void CPanNapUplinkAuthorisationNotifier::Release()
	{
	Cancel();
	delete this;
	}
	
CPanNapUplinkAuthorisationNotifier::TNotifierInfo CPanNapUplinkAuthorisationNotifier::RegisterL()
	{
	return iNotifierInfo;
	}
	
CPanNapUplinkAuthorisationNotifier::TNotifierInfo CPanNapUplinkAuthorisationNotifier::Info() const
	{
	return iNotifierInfo;
	}
	
TPtrC8 CPanNapUplinkAuthorisationNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	// Sync calls not supported.
	User::Leave(KErrNotSupported);
	return KNullDesC8();
	}
	
void CPanNapUplinkAuthorisationNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iMessageOutstanding = ETrue;

	iSelectorEngine->GetLocalServiceL(aBuffer); 
	}
	
void CPanNapUplinkAuthorisationNotifier::Cancel()
	{
	iSelectorEngine->Cancel();
	CompleteNotifier(KErrCancel, EDisallowNewNapConnection);
	}
	
TPtrC8 CPanNapUplinkAuthorisationNotifier::UpdateL(const TDesC8& /*aBuffer*/)	
	{
	User::Leave(KErrNotSupported);
	return KNullDesC8();
	}


	// From MPanNapUplinkAuthorisationNotifierNotifier
void CPanNapUplinkAuthorisationNotifier::CompleteNotifier(TInt aError, TNapConnectionResult aResult)
	{
	if(aError == KErrNone)
		{
		iResultPckg = aResult;
		aError = iMessage.Write(iReplySlot, iResultPckg);
		}

	if(iMessageOutstanding)
		{
		iMessage.Complete(aError);
		iMessageOutstanding = EFalse;
		}
	}



/*static*/ CPanNapUplinkAuthorisationNotifierEngine* CPanNapUplinkAuthorisationNotifierEngine::NewL(MPanNapUplinkAuthorisationNotifierNotifier& aNotifier)
	{
	CPanNapUplinkAuthorisationNotifierEngine* self = new (ELeave)CPanNapUplinkAuthorisationNotifierEngine(aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CPanNapUplinkAuthorisationNotifierEngine::~CPanNapUplinkAuthorisationNotifierEngine()
	{
	delete iConsole;
	}
	
CPanNapUplinkAuthorisationNotifierEngine::CPanNapUplinkAuthorisationNotifierEngine(MPanNapUplinkAuthorisationNotifierNotifier& aNotifier)
  : CActive(CActive::EPriorityStandard),
    iNotifier(aNotifier)
	{
	CActiveScheduler::Add(this);
	}
	
void CPanNapUplinkAuthorisationNotifierEngine::ConstructL()
	{
	}

void CPanNapUplinkAuthorisationNotifierEngine::GetLocalServiceL(const TDesC8& aBuffer)
	{
	iConsole = Console::NewL(KNapUplinkAuthorisationNotifierConsoleName, TSize(KConsFullScreen, KConsFullScreen));
	iConnectionListPckg.Copy(aBuffer);

	DrawExistingConnections();
	DrawInstructionsAndWaitForInput();
	}

void CPanNapUplinkAuthorisationNotifierEngine::DrawExistingConnections()
	{
	TBuf<20> bdAddrBuf;
	const TBTDevAddr* panConnAddr = NULL;
	const TPanConnection* existingPanConnection = NULL;
	
	iConsole->ClearScreen();

	// Get the requested connection address
	panConnAddr = iConnectionListPckg().RequestedConnectionAddr();
	
	if(panConnAddr)
		{
		panConnAddr->GetReadable(bdAddrBuf);
		iConsole->Printf(_L("New connection requested from device [%S]\n"), &bdAddrBuf);
		}
	else
		{
		iConsole->Printf(_L("New connection requested from device [**Unknown**]\n"));
		}

	// Draw existing connections	
	existingPanConnection = iConnectionListPckg().GetFirstRemoteConnection();
	if(existingPanConnection)
		{
		// Print the current local role.
		if(iConnectionListPckg().CurrentLocalRole() == TUUID(KPanUUUID))
			{
			iConsole->Printf(_L("Current local role [PAN-U]\n"));
			}
		else if(iConnectionListPckg().CurrentLocalRole() == TUUID(KPanGnUUID))
			{
			iConsole->Printf(_L("Current local role [PAN-GN]\n"));
			}
		else if(iConnectionListPckg().CurrentLocalRole() == TUUID(KPanNapUUID))
			{
			iConsole->Printf(_L("Current local role [PAN-NAP]\n"));
			}
		else
			{
			iConsole->Printf(_L("Current local role [**Unknown**]\n"));
			}

		iConsole->Printf(_L("\n\nExisting connections\n"));
		iConsole->Printf(_L("\nAddress					Has Uplink Access\n"));

		while(existingPanConnection)
			{
			existingPanConnection->RemoteDeviceAddress().GetReadable(bdAddrBuf);
			iConsole->Printf(_L(" [%S]				%S\n"), &bdAddrBuf, existingPanConnection->UplinkAccessAllowed() ? &KYes : &KNo);

			existingPanConnection = iConnectionListPckg().GetNextRemoteConnection();
			}
		}
	else
		{
		iConsole->Printf(_L("\n -- No existing connections --\n"));
		}
	}

void CPanNapUplinkAuthorisationNotifierEngine::DrawInstructionsAndWaitForInput()
	{
	iConsole->Printf(_L("\n\nPress 'd' to disallow new connection\n"));	
	iConsole->Printf(_L("Press 'n' to give the remote device network access\n"));	
	iConsole->Printf(_L("Press 'l' to give the remote device access to the local network\n"));
	iConsole->Printf(_L("-> "));

	iConsole->Read(iStatus);
	SetActive();
	}
	
void CPanNapUplinkAuthorisationNotifierEngine::RunL()
	{
	TChar key = iConsole->KeyCode();
	switch(key)
		{
		case 'd':
			iNotifier.CompleteNotifier(KErrNone, EDisallowNewNapConnection);
			CloseConsoleWindow();
			break;
			
		case 'l':
			iNotifier.CompleteNotifier(KErrNone, EAcceptNapConnectionDisallowUplinkAccess);
			CloseConsoleWindow();
			break;

		case 'n':
			iNotifier.CompleteNotifier(KErrNone, EAcceptNapConnectionAllowUplinkAccess);
			CloseConsoleWindow();
			break;
		
		default:
			// Invalid character - request input again.
			DrawExistingConnections();
			iConsole->Printf(_L("\n *** Invalid option [%c] Please try again ***\n"), TUint(key));
			DrawInstructionsAndWaitForInput();
			break;
		};
	}
	
void CPanNapUplinkAuthorisationNotifierEngine::DoCancel()
	{
	iConsole->ReadCancel();

	CloseConsoleWindow();
	}

void CPanNapUplinkAuthorisationNotifierEngine::CloseConsoleWindow()
	{
	// Delete the console window
	delete iConsole;
	iConsole = NULL;
	}

