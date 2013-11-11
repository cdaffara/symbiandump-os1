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

#ifndef AGENTDUMMYNOTIFIER_H
#define AGENTDUMMYNOTIFIER_H

#include <twintnotifier.h>
#include "agentnotifierdefs.h"

const TUid KUidDummyAgentDialogNotifier = { 0x101FEB92 };

IMPORT_C CArrayPtr<MNotifierBase2>* NotifierArray();

class CAgentDummyNotifier : public MNotifierBase2, public CBase
{
public:

	/**
	 * Construction/destruction
	 */
	static CAgentDummyNotifier* NewL();
	~CAgentDummyNotifier();

	/**
	 * Called when all resources allocated by notifiers should be freed.
	 */
	virtual void Release();
	/**
	 * Called when a notifier is first loaded to allow any initial construction that is required.
	 */
	virtual TNotifierInfo RegisterL();
	/**
	 * Return the priority a notifier takes and the channels it acts on.  The return value may be varied 
	 * at run-time.
	 */
	virtual TNotifierInfo Info() const;
	/**
	 * Start the notifier with data aBuffer and return an initial response.
	 */
	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	/**
	 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
	 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
	 * responsible for completing aMessage.
	 */
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	/**
	 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
	 */
	virtual void Cancel();
	/**
	 * Update a currently active notifier with data aBuffer.
	 */
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);

private:
	/**
	 * Private construction stuff
	 */
	CAgentDummyNotifier();
	void ConstructL();
	
	/*
	 * Private data
	 */
	TNotifierInfo iInfo;
};

#endif

