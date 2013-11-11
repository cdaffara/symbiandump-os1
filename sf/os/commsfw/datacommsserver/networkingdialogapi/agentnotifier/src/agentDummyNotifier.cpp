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

#include <e32std.h>
#include <e32base.h>
#include "agentnotifierdefs.h"
#include "agentDummyNotifier.h"

EXPORT_C CArrayPtr<MNotifierBase2>* NotifierArray()
//
// Lib main entry point
//
	{
	CArrayPtrFlat<MNotifierBase2>* subjects=new (ELeave)CArrayPtrFlat<MNotifierBase2>(1);
	CleanupStack::PushL(subjects);
	subjects->AppendL(CAgentDummyNotifier::NewL());
	CleanupStack::Pop();//subjects
	return(subjects);
	}


/*
 * CAgentDummyNotifier
 */

CAgentDummyNotifier* CAgentDummyNotifier::NewL()
	{
	CAgentDummyNotifier* self = new (ELeave) CAgentDummyNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CAgentDummyNotifier::CAgentDummyNotifier()
	{
	}

void CAgentDummyNotifier::ConstructL()
	{
	}

CAgentDummyNotifier::~CAgentDummyNotifier()
	{
	}

void CAgentDummyNotifier::Release()
	{
	delete this;
	}

CAgentDummyNotifier::TNotifierInfo CAgentDummyNotifier::RegisterL()
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 */
	{
	iInfo.iUid=KUidDummyAgentDialogNotifier;
	iInfo.iChannel=KUidDummyAgentDialogNotifier;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CAgentDummyNotifier::TNotifierInfo CAgentDummyNotifier::Info() const
	/**
	 * Return the previously generated info about the notifier
	 */
	{
	return iInfo;
	}

TPtrC8 CAgentDummyNotifier::StartL(const TDesC8& /*aBuffer*/)
	/**
	 * Start the notifier with data aBuffer.
	 * May be called multiple times if more than one client starts the notifier.
	 */
	{
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}
	
void CAgentDummyNotifier::StartL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 */
	{
	// just complete the message
	aMessage.Complete(EAgentYesPressed); // assume yes is the right answer
	}

void CAgentDummyNotifier::Cancel()
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 */
	{
	}

TPtrC8 CAgentDummyNotifier::UpdateL(const TDesC8& /*aBuffer*/)
	/**
	 * Update a currently active notifier with data aBuffer.
	 */
	{
	TPtrC8 ret(KNullDesC8);
	return (ret);
	}

