// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bttypes.h>
#include <e32cons.h>
#include "BTTextNotifiers.h"

#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
//define password always returned by auth notifier
_LIT(KAutoPassword, "1234");
#endif

//
// PbapTextNotifiersConsole
//
CConsoleBase* PbapTextNotifiersConsole::AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	//try to create a console of our preferred size, otherwise we
	//guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		//if we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}

//
// CPbapAuthNotifier
//
CPbapAuthNotifier* CPbapAuthNotifier::NewLC()
	{
	CPbapAuthNotifier* self = new (ELeave) CPbapAuthNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPbapAuthNotifier::CPbapAuthNotifier()
	{
	}

void CPbapAuthNotifier::ConstructL()
	{
	iEngine = CPbapAuthNotifierEngine::NewL(*this);
	}

void CPbapAuthNotifier::Release()
	{
	Cancel();
	delete this;
	}

void CPbapAuthNotifier::Cancel()
	{
	iEngine->Stop();

	if (iNeedToCompleteMessage)
		{
		iMessage.Complete(KErrCancel);
		iNeedToCompleteMessage = EFalse;
		}
	}

CPbapAuthNotifier::~CPbapAuthNotifier()
	{
	delete iEngine;
	}

CPbapAuthNotifier::TNotifierInfo CPbapAuthNotifier::RegisterL()
	{
	iInfo.iUid=KPbapAuthNotifierUid;
	iInfo.iChannel=KScreenOutputChannel;
	iInfo.iPriority=ENotifierPriorityVHigh;
	return iInfo;
	}

CPbapAuthNotifier::TNotifierInfo CPbapAuthNotifier::Info() const
	{
	return iInfo;
	}

TPtrC8 CPbapAuthNotifier::StartL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPbapAuthNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	iReplySlot = aReplySlot;
	iMessage = RMessage2(aMessage);
	iNeedToCompleteMessage = ETrue;

	TRAPD(err, iEngine->StartPasswordEntryL(aBuffer));
	if (err)
		{
		aMessage.Complete(err);
		iNeedToCompleteMessage = EFalse;
		User::Leave(err);
		}
	}

TPtrC8 CPbapAuthNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	{
	return KNullDesC8();
	}

void CPbapAuthNotifier::PasswordInputComplete(const TDesC& aPassword, TInt aReason)
	{
	if (aReason == KErrNone)
		{
		TPbapAuthNotifierResponsePckg pckg;
		pckg().SetPassword(aPassword);		
		TInt err = iMessage.Write(iReplySlot, pckg);
		iMessage.Complete(err);
		}
	else
		{
		iMessage.Complete(aReason);
		}
	iNeedToCompleteMessage = EFalse;
	}


//
// CPbapAuthNotifierEngine
//
CPbapAuthNotifierEngine* CPbapAuthNotifierEngine::NewL(CPbapAuthNotifier& aParent)
	{
	CPbapAuthNotifierEngine* self = new (ELeave) CPbapAuthNotifierEngine(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbapAuthNotifierEngine::CPbapAuthNotifierEngine(CPbapAuthNotifier& aParent)
: CActive(EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

void CPbapAuthNotifierEngine::ConstructL()
	{
	}

void CPbapAuthNotifierEngine::StartPasswordEntryL(const TDesC8& aBuffer)
	{
	iParamsPckg.Copy(aBuffer);

	delete iDevice;
	iDevice = NULL;
	iDevice = CBTDevice::NewL(iParamsPckg().RemoteAddr());

	// get the device name from the registry
	User::LeaveIfError(iRegistry.Connect());
	User::LeaveIfError(iRegistryView.Open(iRegistry));
	iRegistrySearch.FindAddress(iParamsPckg().RemoteAddr());
	iRegistryView.CreateView(iRegistrySearch, iStatus);
	iRegistryState = EFinding;
	SetActive();
	}

void CPbapAuthNotifierEngine::RunL()
	{
	if (iStatus == KErrNotFound)
		{
		// name not found, need to store device address so it can be displayed in UI 
		iDevice->SetDeviceAddress(iParamsPckg().RemoteAddr());
		iAuthConsole = CPbapAuthNotifierConsole::NewL(this, *iDevice, iParamsPckg().Realm(), iParamsPckg().RealmTruncated());
		iAuthConsole->GetPasswordL();
		}
	else if (iStatus >= KErrNone)
		{
		switch (iRegistryState)
			{
			case EFinding:
				{
				iResponse = CBTRegistryResponse::NewL(iRegistryView);
				iResponse->Start(iStatus);
				SetActive();
				iRegistryState = EGetting;
				}
				break;
			case EGetting:
				{
				iAuthConsole = CPbapAuthNotifierConsole::NewL(this, *(iResponse->Results()[0]), iParamsPckg().Realm(), iParamsPckg().RealmTruncated());
				iAuthConsole->GetPasswordL();
				}
				break;
			}
		}
	}

void CPbapAuthNotifierEngine::DoCancel()
	{
	iRegistryView.CancelRequest(iStatus);
	if (iResponse)
		{
		iResponse->Cancel();
		}
	if (iAuthConsole)
		{
		iAuthConsole->Cancel();
		}
	}

CPbapAuthNotifierEngine::~CPbapAuthNotifierEngine()
	{
	Cancel();

	delete iAuthConsole;
	delete iDevice;

	iRegistryView.Close();
	iRegistry.Close();
	}

void CPbapAuthNotifierEngine::Stop()
	{
	Cancel();

	if (iAuthConsole)
		{
		iAuthConsole->Cancel();
		delete iAuthConsole;
		iAuthConsole = NULL;
		}

	//clean up device
	delete iDevice;
	iDevice = NULL;
	}

void CPbapAuthNotifierEngine::PasswordInputComplete(const TDesC& aPassword, TInt aReason)
	{
	iParent.PasswordInputComplete(aPassword, aReason);
	delete iDevice;
	iDevice = NULL;
	delete iAuthConsole;
	iAuthConsole = NULL;
	}


//
// CPbapAuthNotifierConsole
//
CPbapAuthNotifierConsole* CPbapAuthNotifierConsole::NewL(CPbapAuthNotifierEngine* aParent, const CBTDevice& aDevice, const TDesC& aRealm, TBool aRealmTruncated)
	{
	CPbapAuthNotifierConsole* self = new(ELeave) CPbapAuthNotifierConsole(aParent, aRealm, aRealmTruncated);
	CleanupStack::PushL(self);
	self->ConstructL(aDevice);
	CleanupStack::Pop(self);
	return self;
	}

CPbapAuthNotifierConsole::CPbapAuthNotifierConsole(CPbapAuthNotifierEngine* aParent, const TDesC& aRealm, TBool aRealmTruncated)
	: CActive(EPriorityStandard), iParent(aParent), iRealm(aRealm), iRealmTruncated(aRealmTruncated)
	{
	CActiveScheduler::Add(this);
	}

void CPbapAuthNotifierConsole::ConstructL(const CBTDevice& aDevice)
	{
	iConsole = PbapTextNotifiersConsole::AutoSizeNewL(_L("PBAP Password input"), TSize(KConsFullScreen,KConsFullScreen));
	iDevice = &aDevice;

#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	TCallBack autoNotifierCB(AutoNotifierCallBack, this);
	iAutoNotifierCallback = new (ELeave)CAsyncCallBack(autoNotifierCB, EPriorityStandard);
#endif // __PBAP_TEXT_NOTIFIERS_AUTO__
	}

CPbapAuthNotifierConsole::~CPbapAuthNotifierConsole()
	{
	Cancel();
	delete iConsole;
#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	delete iAutoNotifierCallback;
#endif
	}

void CPbapAuthNotifierConsole::RunL()
	{
#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	iParent->PasswordInputComplete(KAutoPassword(), KErrNone);
#else // __PBAP_TEXT_NOTIFIERS_AUTO__

	TKeyCode key = iConsole->KeyCode();
	if (key == EKeyEnter)
		{
		iParent->PasswordInputComplete(iPassword, KErrNone);
		}
	else if (key == EKeyEscape)
		{
		iParent->PasswordInputComplete(iPassword, KErrCancel);
		}
	else
		{
		if (iPassword.Length() < KPbapAuthPasswordLength)
			{
			TBuf<1> b;
			b.Append(key);
			iPassword.Append(b);
			iConsole->Printf(b);
			IssueRequestL();
			}
		else
			{
			iConsole->Printf(_L("\nToo many characters...\nPress any key to continue\n"));
			TRequestStatus stat;
			iConsole->Read(stat);
			User::WaitForRequest(stat);
			GetPasswordL();
			}
		}
#endif // __PBAP_TEXT_NOTIFIERS_AUTO__
	}

void CPbapAuthNotifierConsole::DoCancel()
	{
#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	iConsole->ReadCancel();
#endif
	iConsole->Printf(_L("\nPin cancelled.\nPress any key to continue...\n"));
	iPassword.Zero();
	}

void CPbapAuthNotifierConsole::GetPasswordL()
	{
	iPassword.Zero();
	iConsole->ClearScreen();
	iConsole->Printf(_L("Please enter password for device:\nAddress: "));
	TBTDevAddr a = iDevice->BDAddr();
	iConsole->Printf(_L("0x%02x%02x%02x%02x%02x%02x"), a[0], a[1], a[2], a[3], a[4], a[5]);
	if (iDevice->IsValidDeviceName())
		{
		iConsole->Printf(_L("\nName: "));
		TBuf<KMaxBluetoothNameLen> dispBuf;	 // BT allows device names upto 248 characters
		dispBuf.Copy(iDevice->DeviceName());
		iConsole->Printf(dispBuf);
		}
	
	iConsole->Printf(_L("\nRealm: "));
	iConsole->Printf(iRealm);
	if (iRealmTruncated)
		{
		iConsole->Printf(_L(">>"));
		}
		
	iConsole->Printf(_L("\nand press [return] to accept or [esc] to cancel\n"));
	IssueRequestL();
	}

void CPbapAuthNotifierConsole::IssueRequestL()
	{
#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	iAutoNotifierCallback->CallBack();
	iStatus = KRequestPending;
#else //__PBAP_TEXT_NOTIFIERS_AUTO__
	iConsole->Read(iStatus);
#endif // __PBAP_TEXT_NOTIFIERS_AUTO__

	SetActive();
	}

#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
/*static*/ TInt CPbapAuthNotifierConsole::AutoNotifierCallBack(TAny *aAuthConsole)
	{
	CPbapAuthNotifierConsole* authConsole = static_cast<CPbapAuthNotifierConsole*>(aAuthConsole);
	if(authConsole->IsActive())
		{
		TRequestStatus* stat = &(authConsole->iStatus);
		User::RequestComplete(stat, KErrNone);
		}
	return EFalse;
	}
#endif // __PBAP_TEXT_NOTIFIERS_AUTO__
