// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cons.h>

#include <es_sock.h>

#include "agentnotifierdefs.h"
#include "agentshellnotifier.h"

EXPORT_C CArrayPtr<MNotifierBase2>* NotifierArray()
//
// Lib main entry point
//
	{
	CArrayPtrFlat<MNotifierBase2>* subjects=new (ELeave)CArrayPtrFlat<MNotifierBase2>(1);
	CleanupStack::PushL(subjects);
	subjects->AppendL(CAgentShellNotifier::NewL());
	CleanupStack::Pop();//subjects
	return(subjects);
	}



const TInt KAgentDialogWidth=38;
const TInt KAgentDialogHeight=4;
__FLOG_STMT(_LIT8(KDlgLogSubsys, "agentdialog");)
__FLOG_STMT(_LIT8(KDlgLogComponent, "notifier");)

CAgentDialogUpdate* CAgentDialogUpdate::NewL(const RMessagePtr2& aMessage)
	{
	CAgentDialogUpdate* self=new (ELeave) CAgentDialogUpdate(aMessage);
	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	CleanupStack::Pop();
	return self;
	}
	
void CAgentDialogUpdate::Update(const RMessagePtr2& aMessage)
	{
	__FLOG(_L("CAgentDialogUpdate::Update Starting Agent Shell Dialog"));
	iMessagePtr = aMessage;
	TAgentDialogInputBuf agentDialogInputBuf;
	aMessage.ReadL(1, agentDialogInputBuf);
	TAgentDialogInput& agentdialogInput = agentDialogInputBuf();

	iConsole->ClearScreen();
	iConsole->SetTitle(agentdialogInput.iTitle);

	TBuf<KAgentDialogWidth> buf1;
	TBuf<KAgentDialogWidth> buf2;
	buf1.Append(agentdialogInput.iLabel);
	if(buf1.Length() >= 44 )
	{
		buf1.SetLength(40);
		buf1.AppendFill('*',3);
	}
	buf1.Append(' ');

	_LIT(KYesNoChoice, "Yes [y] No [n] Cancel[c]");
	_LIT(KContinuePrompt, "Continue Yes [y]");

	TInt operation = agentdialogInput.getDialogOperation();    
	switch ( operation )
		{
		case ENoYesDialog:
			buf2.Append(KYesNoChoice);
			break;
		case EContinueDialog:
			buf2.Append(KContinuePrompt);
			break;
		default:
		__ASSERT_DEBUG(EFalse, _L("Illegal Agent UI operation type"));
		}

	TPoint point;
	TInt x = (iConsole->ScreenSize().iWidth - buf1.Length()) / 2; // put the string in the middle
	point.SetXY(x, 0);
	iConsole->SetCursorPosAbs(point);

	iConsole->Printf(buf1);
	x = (iConsole->ScreenSize().iWidth - buf2.Length()) / 2; // put the string in the middle
	point.SetXY(x, 1);
	iConsole->SetCursorPosAbs(point);
	iConsole->Printf(buf2);
	}
	
void CAgentDialogUpdate::DoCancel()
	{
	if (IsActive())
		{
		if (!iMessagePtr.IsNull())
			{
			iMessagePtr.Complete(KErrCancel);
			}
		iConsole->ReadCancel();
		iConsole->Printf(_L("\ndialog cancelled.\nPress any key to continue...\n"));
		}

	}

void CAgentDialogUpdate::RunL()
	{
	TInt key = iConsole->KeyCode();
	__FLOG_1(_L("KeyPressed : %c"),key);
	switch (key)
		{
		case 'y':
		case 'Y':
			iMessagePtr.Complete(EAgentYesPressed);
			break;
		case 'n':
		case 'N':
			iMessagePtr.Complete(EAgentNoPressed);
			break;
		case 'c':
		case 'C':
			iMessagePtr.Complete(KErrCancel);
			break;
		default:
			iConsole->Printf(_L("\n Press 'y' 'n' or 'c'\n"));
			iConsole->Read(iStatus);
			SetActive();
		}
	}

CAgentDialogUpdate::CAgentDialogUpdate(const RMessagePtr2& aMessage)
:	CActive(EPriorityStandard),
	iMessagePtr(aMessage)
	{
	__FLOG_OPEN(KDlgLogSubsys, KDlgLogComponent);
	}

void CAgentDialogUpdate::ConstructL(const RMessagePtr2& /*aMessage*/)
	{
	TRAPD(r,iConsole = Console::NewL(_L("AgentDialog"), TSize(KAgentDialogWidth,KAgentDialogHeight)));
	__FLOG_1(_L("CAgentDialogUpdate::ConstructL - Console::NewL returned %d"),r);
	
	if (r < KErrNone) //So why not User::LeaveIfError - well Console::NewL is know to return 2
		{
		User::Leave(r);
		}
		if (iConsole == NULL)
		{
		User::Leave(KErrGeneral);
		}
	CActiveScheduler::Add(this);

	// Read back the response
	iConsole->Read(iStatus);
	SetActive();
	__FLOG_1(_L("Performed read for response should still be KRequestPending - %x"),iStatus.Int());

	}

CAgentDialogUpdate::~CAgentDialogUpdate()
	{
	Cancel();
	delete iConsole;
	__FLOG_CLOSE;
	}
	


CAgentShellNotifier* CAgentShellNotifier::NewL()
	{
	CAgentShellNotifier* self=new (ELeave) CAgentShellNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CAgentShellNotifier::ConstructL()
	{
	}

CAgentShellNotifier::~CAgentShellNotifier()
	{
	delete iAgentShellDialog;
	}
	//	From MNotifierBase

void CAgentShellNotifier::Release()
	{
	delete this;
	}

CAgentShellNotifier::TNotifierInfo CAgentShellNotifier::RegisterL()
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 */
	{
	iInfo.iUid=KUidAgentDialogNotifier;
	iInfo.iChannel=KUidAgentDialogNotifier;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CAgentShellNotifier::TNotifierInfo CAgentShellNotifier::Info() const
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 */
	{
	return iInfo;
	}

TPtrC8 CAgentShellNotifier::StartL(const TDesC8& /*aBuffer*/)
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 */
	{
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}
	
void CAgentShellNotifier::StartL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 */
	{
	//	Keep this short! Must schedule an update and return immediately
	if (!aMessage.IsNull())
		{
		if (iAgentShellDialog)
			{
			delete iAgentShellDialog;
			iAgentShellDialog = NULL;
			}
		iAgentShellDialog = CAgentDialogUpdate::NewL(aMessage);

		iAgentShellDialog->Update(aMessage);
		}
	}


void CAgentShellNotifier::Cancel()
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 */
	{
	if (iAgentShellDialog)
		{
		iAgentShellDialog->Cancel();
		delete iAgentShellDialog;
		iAgentShellDialog=NULL;
		}
	}

TPtrC8 CAgentShellNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	/**
	 * Update a currently active notifier with data aBuffer.
	 */
	{
	//	Keep this short! Must schedule an update and return immediately
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}



